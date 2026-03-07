#include "Preprocessor.hpp"
#include <cassert>
#include <string.h>
#include <stdarg.h>

constexpr int32_t HIT_ENDIF = 1;
constexpr int32_t HIT_ELSE = 2;
constexpr int32_t HIT_ELSEIF = 3;

struct Headername
{
    std::string_view name;
    uint8_t isLocal : 1; // include from file directory
};

Preprocessor::Preprocessor(FILE_STATE mainFile, FileManager *manager, const CompilationOpts* opts)
:
lexer(mainFile, manager, opts), manager(manager), opts(opts), stages({})
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

void Preprocessor::ExecuteConstantExpr(Ast::Node *expr)
{
}

int32_t Preprocessor::ExecuteDirective(Token *token)
{
    Token ppToken = GetCurrToken();
    ConsumeToken();

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
    case TokenType::pp_undef:   HandleUndef(); break;
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
    if(loc)
    {
        printf("%ld:%ld", loc->line, loc->offset);
    }
    printf(" Preprocessor warning \n");

    if(errMsg)
    {
        va_list args;
        va_start(args, errMsg);
        vprintf(errMsg, args);
        va_end(args);
    }
    printf("\n");
    
}

std::string_view Preprocessor::GetViewForToken(const Token &token)
{
    FILE_STATE state;
    if(manager->GetFileState(&token.location.id, &state) != 0)
    {
        printf("Preprocessor critical error: Requested file does not exit\n");
        exit(-1);
    }

    // removes \" from both start and end 
    uint8_t offset = token.type == TokenType::string_literal ? 1 : 0;
    std::string_view tokenView(state.fileData + token.location.offset + offset,
                                token.location.len - offset);
    return tokenView;
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

void Preprocessor::PutBackAtFront(Token token)
{
    tokenQueue.push_front(token);
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
        /* TODO verify later
        if(!IsTokenOneOf(&buffToken,TokenType::identifier, TokenType::slash, TokenType::dot, TokenType::greater))
        {
            IssueWarning(&buffToken.location.id, &buffToken.location, "Incorrect header name");
            exit(-1);
        }
        */

    } while (buffToken.type != TokenType::greater);
    ConsumeToken();

    const char* basePointer = state.fileData + firstToken.location.offset;
    int64_t len = lastToken.location.len +  (lastToken.location.offset - firstToken.location.offset);
    std::string_view headerView(basePointer, len);
    return headerView;
}

int32_t Preprocessor::HandleIf()
{
    stages.If = 1;
    return 0;
}

int32_t Preprocessor::HandleElse()
{
    if(conditionalBlocks.empty())
    {
        IssueWarning(nullptr, nullptr, "#endif used outsite of #if block\n");
        exit(-1);
    }

    if(conditionalBlocks.top().doneIncluding)
    {
        Token info;
        int32_t ret = SkipTokensInBlock(&info);
        if(ret != HIT_ENDIF)
        {
            IssueWarning(&info.location.id,&info.location, "#else block may only end with #endif\n" );
            exit(-1);
        }
    }
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
        printf("local include are not supported \n");
        exit(-1);
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
    Token defineIdentifier = GetCurrToken();
    ConsumeToken();

    Token macroExpansion = GetCurrToken();

    Macro macro = {};
    std::string_view macroName = GetViewForToken(defineIdentifier);
    // check if macro is callable
    if(macroExpansion.type == TokenType::l_parentheses && 
        macroExpansion.skippedHorizWhitespace > 0)
    {
        exit(-1);
    }
    else if(macroExpansion.type != TokenType::new_line)
    {
        do
        {
            if(macroExpansion.type != TokenType::line_splice)
            {
                macro.tokenList.push_back(macroExpansion);
            }
            ConsumeToken();
            macroExpansion = GetCurrToken();
        }while (macroExpansion.type != TokenType::new_line);

    }

    macros[macroName] = std::move(macro);
    return 0;
}

int32_t Preprocessor::HandleIfdef()
{
    Token identifier = GetCurrToken();
    ConsumeExpectedToken(TokenType::identifier);
    ConsumeExpectedToken(TokenType::new_line);
    std::string_view macroView = GetViewForToken(identifier);

    auto macroIter = macros.find(macroView);
    ConditionalBlock block = CreateBlock(); 
    if(macroIter == macros.end())
    {
        SkipTokensInBlock();
        block.doneIncluding = false;
    }
    else {block.doneIncluding = true;}

    conditionalBlocks.push(block);
    return 0;
}

int32_t Preprocessor::HandleIfndef()
{
    Token identifier = GetCurrToken();
    ConsumeExpectedToken(TokenType::identifier);
    ConsumeExpectedToken(TokenType::new_line);
    std::string_view macroView = GetViewForToken(identifier);

    auto macroIter = macros.find(macroView);
    ConditionalBlock block = CreateBlock(); 
    if(macroIter != macros.end())
    {
        SkipTokensInBlock();
        block.doneIncluding = false;
    }
    else {block.doneIncluding = true;}


    conditionalBlocks.push(block);
    return 0;
}

int32_t Preprocessor::HandleElif()
{
    if(conditionalBlocks.empty())
    {
        IssueWarning(nullptr, nullptr, "#elif used outsite of #if block\n");
        exit(-1);
    }

    if(conditionalBlocks.top().doneIncluding)
    {
        Token info;
        SkipTokensInBlock(&info);

        return 0;
    }
    // TODO add argument evaluatio
    return 0;
}

int32_t Preprocessor::HandleEndif()
{
    Token token =  GetCurrToken(); // token used in logging only
    ConsumeExpectedToken(TokenType::new_line);
    if(conditionalBlocks.empty())
    {
        IssueWarning(&token.location.id, &token.location, "#endif used without #if");
        exit(-1);
    }
    conditionalBlocks.pop();
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
    Token pragmaType = GetCurrToken();
    ConsumeToken();
    if(pragmaType.type == TokenType::new_line)
    {
        printf("Empty pragma\n");
    }

    std::string_view command = GetViewForToken(pragmaType);
    if(command == "GCC")
    {
        Token pragmaSubType = GetCurrToken();
        ConsumeToken();

        std::string_view subCommand = GetViewForToken(pragmaSubType);
        if(subCommand == "warning")
        {
            Token tokenWarning = GetCurrToken();
            ConsumeToken();
            std::string_view subCommand = GetViewForToken(tokenWarning);
            char* msg = (char*) alloca(subCommand.length() + 1);
            memcpy(msg, subCommand.data(), subCommand.length());
            msg[subCommand.length()] = '\0';
            IssueWarning(&tokenWarning.location.id, &tokenWarning.location, msg);
        }
        else
        {
            printf("Unsupported pragma subtype\n");
            exit(-1);
        }
    }
    else
    {
        printf("Unsupported pragma type\n");
        exit(-1);
    }
    return 0;
}

int32_t Preprocessor::HandleUndef()
{
    Token defineIdentifier = GetCurrToken();
    ConsumeExpectedToken(TokenType::identifier);
    ConsumeExpectedToken(TokenType::new_line);

    std::string_view macroName = GetViewForToken(defineIdentifier);
    macros.erase(macroName);

    return 0;
}

int32_t Preprocessor::SkipTokensInBlock(Token* infoToken)
{
    Token token = GetCurrToken();
    uint64_t nestedIfs = 0;
    while (token.type != TokenType::eof)
    {
        token = GetCurrToken();
        if(token.type == TokenType::hash)
        {
            // consume '#' 
            ConsumeToken();
            Token ppToken = GetCurrToken();
            if(nestedIfs == 0 && ppToken.type == TokenType::kw_else)
            {
                // if we hit else/elif we want to keep # in queue
                if(infoToken){*infoToken = ppToken;}
                PutBackAtFront(token);
                return HIT_ELSE;
            }
            else if(nestedIfs == 0 && ppToken.type == TokenType::pp_elif)
            {
                // if we hit else/elif we want to keep # in queue
                if(infoToken){*infoToken = ppToken;}
                PutBackAtFront(token);
                return HIT_ELSEIF;
            }
            if(nestedIfs == 0 && ppToken.type == TokenType::pp_endif)
            {
                if(infoToken){*infoToken = ppToken;}
                PutBackAtFront(token);
                return HIT_ENDIF;
            }
            else if(ppToken.type == TokenType::pp_endif)
            {
                nestedIfs--;
            }
            else if(IsTokenOneOf(&ppToken, TokenType::kw_if, TokenType::pp_ifdef, TokenType::pp_ifndef))
            {
                nestedIfs++;
            }

        }
        ConsumeToken();

    }
    if(infoToken){*infoToken = token;} 
    IssueWarning(&token.location.id, &token.location, "Block is not enclosed with #endif\n");
    exit(-1);
    return 0;
}

ConditionalBlock Preprocessor::CreateBlock()
{
    ConditionalBlock block;
    if(conditionalBlocks.size() == 0)
    {
        block.nestLevel = 0;
    }
    else
    {
        block.nestLevel = conditionalBlocks.top().nestLevel + 1;
    }
    block.doneIncluding = false;
    return block;
}
