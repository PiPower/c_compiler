#include "Preprocessor.hpp"
#include <cassert>
#include <string.h>
#include <stdarg.h>

struct Headername
{
    std::string_view name;
    uint8_t isLocal : 1;
};

template<typename... Args>
static bool IsTokenOneOf(const Token* token, Args&&... args)
{
    return ((token->type == args) || ...);
}


Preprocessor::Preprocessor(FILE_STATE mainFile, FileManager *manager, const CompilationOpts* opts)
:
lexer(mainFile, manager, opts), manager(manager), opts(opts)
{
    assert(opts != nullptr);
}

int32_t Preprocessor::Peek(Token* token)
{
fetch_token:
    do
    {
        *token = GetCurrToken();
        ConsumeToken();
    } while (token->type == TokenType::comment || token->type == TokenType::new_line);
    
    if(token->type == TokenType::hash)
    {
        int32_t ret = ExecuteDirective(token);
        if(ret != 0)
        {
            return ret;
        }
        goto fetch_token;
    }

    return 0;
}


int32_t Preprocessor::ExecuteDirective(Token *token)
{
    Token ppToken = GetCurrToken();
    ConsumeToken();
    if(ppToken.skippedHorizWhitespace > 0)
    {
        IssueWarning(&ppToken.location.id, &ppToken.location,
            "Directive separated by white space is not allowed \n");
        exit(-1);
    }

    switch (ppToken.type)
    {
    case TokenType::kw_if:      HandleIf(); break;
    case TokenType::kw_else:    HandleElse();break;
    case TokenType::pp_include: HandleInclude(); break;
    case TokenType::pp_define:  HandleDefine(); break;
    case TokenType::pp_ifdef:   HandleIfdef(); break;
    case TokenType::pp_ifndef:  HandleIfndef(); break;
    case TokenType::pp_elif:    HandleElif(); break;
    case TokenType::pp_endif:   HandleEndif(); break;
    case TokenType::pp_line:    HandleLine(); break;
    case TokenType::pp_error:   HandleError(); break;
    case TokenType::pp_pragma:  HandlePragma(); break;
    default:
        printf("Not expected preprocessing token \n");
        exit(-1);
        break;

    }
    return 0;
}

void Preprocessor::IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char *errMsg, ...)
{
    if(fileId)
    {
        FILE_STATE fileState;
        manager->GetFileState(fileId, &fileState);
        char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
        memcpy(pathBuffer, fileState.path, fileState.pathLen);
        pathBuffer[fileState.pathLen] = '\0';
        printf("%s:", pathBuffer);
    }
    printf("%ld:%ld Preprocessor error: ", loc->line, loc->offset);
    if(errMsg)
    {
        va_list args;
        va_start(args, errMsg);
        vprintf(errMsg, args);
        va_end(args);
    }
    printf("\n");
    
}

Token Preprocessor::GetCurrToken()
{
    if(tokenQueue.empty())
    {
        Token token;
        lexer.Lex(&token);
        tokenQueue.push_back(token);
    }
    return tokenQueue.front();
}

void Preprocessor::ConsumeToken()
{
    if(!tokenQueue.empty())
    {
        tokenQueue.pop_front();
    }
}

void Preprocessor::ConsumeExpectedToken(TokenType::Type type)
{
    Token token = GetCurrToken();
    ConsumeToken();
    if(token.type != type)
    {
        IssueWarning(&token.location.id, &token.location, 
        "Given token is [%s] but expected is [%s]", 
        TokenType::tokenStr(token.type), TokenType::tokenStr(type));
        exit(-1);
    }
    return;
}

std::string_view Preprocessor::FormHeadername()
{
    Token firstToken = GetCurrToken();
    FILE_STATE state;
    manager->GetFileState(&firstToken.location.id, &state);

    Token lastToken;
    Token buffToken = GetCurrToken();
    do
    {
        lastToken = buffToken;
        ConsumeToken();
        buffToken = GetCurrToken();
    
        if(!IsTokenOneOf(&buffToken,TokenType::identifier, TokenType::slash, TokenType::dot, TokenType::greater))
        {
            IssueWarning(&buffToken.location.id, &buffToken.location, "Incorrect header name");
            exit(-1);
        }


    } while (buffToken.type != TokenType::greater);
    ConsumeToken();

    const char* basePointer = state.fileData + firstToken.location.offset;
    int64_t len = lastToken.location.len +  (lastToken.location.offset - firstToken.location.offset);
    std::string_view headerView(basePointer, len);
    return headerView;
}

int32_t Preprocessor::HandleIf()
{
    return 0;
}

int32_t Preprocessor::HandleElse()
{
    return 0;
}

/**
* @brief parses and executes 
* #include <h-char-sequence> new-line or #include "q-char-sequence" new-line
*  general version #include pp-tokens new-line is not allowed 
*/
int32_t Preprocessor::HandleInclude()
{
    Headername header;
    Token headerToken = GetCurrToken();
    ConsumeToken();
    if(headerToken.type == TokenType::less)
    {
        header.name = FormHeadername();
        header.isLocal = 0;
    }
    else if(headerToken.type == TokenType::string_literal)
    {
        FILE_STATE state;
        manager->GetFileState(&headerToken.location.id, &state);

        const char* stringStart = state.fileData + headerToken.location.offset + 1;
        size_t stringLen = headerToken.location.len - 1;
        header.name = std::string_view(stringStart, stringLen); 
        header.isLocal = 1;
    }
    else
    {
        IssueWarning(&headerToken.location.id, &headerToken.location, "Incorrect file include format");
        exit(-1);
    }
    ConsumeExpectedToken(TokenType::new_line);

    char* pathBuffer = (char*)alloca(opts->longestPath + header.name.length() + 2);
    FILE_ID headerFileId;
    for(size_t i = 0; i < opts->searchPaths.size(); i++)
    {
        uint64_t offset = 0;
        memcpy(pathBuffer + offset, opts->searchPaths[i].data(), opts->searchPaths[i].length() );
        offset += opts->searchPaths[i].length();
        // if path does not contain / add it 
        if(pathBuffer[offset - 1] != '/')
        {
            pathBuffer[offset] = '/';
            offset++;
        }

        memcpy(pathBuffer + offset, header.name.data(), header.name.length() );
        offset +=  header.name.length();
        pathBuffer[offset] = '\0';
        int32_t ret = manager->TryLoadFile(pathBuffer, 
            opts->searchPaths[i].length() +  header.name.length() + 1, &headerFileId);
        if(ret == 0)
        {
            break;
        }
    }
    lexer.PushFile(headerFileId);
    return 0;
}

int32_t Preprocessor::HandleDefine()
{
    return 0;
}

int32_t Preprocessor::HandleIfdef()
{
    return 0;
}

int32_t Preprocessor::HandleIfndef()
{
    return 0;
}

int32_t Preprocessor::HandleElif()
{
    return 0;
}

int32_t Preprocessor::HandleEndif()
{
    return 0;
}

int32_t Preprocessor::HandleLine()
{
    return 0;
}

int32_t Preprocessor::HandleError()
{
    return 0;
}

int32_t Preprocessor::HandlePragma()
{
    return 0;
}