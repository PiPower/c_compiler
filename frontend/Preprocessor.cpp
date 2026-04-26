#include "Preprocessor.hpp"
#include <cassert>
#include <string.h>
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <functional>
#include <iostream>
static const char* PreprocessorFlename = "preprocessor_file.comp";
static const char* InsertableValues = "01";

constexpr int32_t HIT_ENDIF = 1;
constexpr int32_t HIT_ELSE = 2;
constexpr int32_t HIT_ELSEIF = 3;

constexpr int32_t EXPR_RESULT_NONE = 2;
constexpr int32_t EXPR_RESULT_FALSE = 0;
constexpr int32_t EXPR_RESULT_TRUE = 1;


struct Headername
{
    std::string_view name;
    uint8_t isLocal : 1; // include from file directory
};

struct MacroTokenQueue
{
    size_t currentElement;
    const Macro* macro;
};


Preprocessor::Preprocessor(FILE_ID mainFileId, FileManager *manager, const CompilationOpts* opts)
:
lexer(manager, opts), manager(manager), opts(opts), stages({}), blockResult(EXPR_RESULT_NONE), fileOffset(0), ex(manager)
{
    assert(opts != nullptr);
    constexpr size_t initiialBufferSize = 500;
    constantNodes.reserve(initiialBufferSize);
    manager->CreateInternalFile(PreprocessorFlename, 
        strlen(PreprocessorFlename), 4096 * 4, &preprocessorFile);
    WriteToPreprocessorFile(InsertableValues, 2);

    lexer.PushFile(mainFileId, -1, -1);
    PushInitFile();
}

int32_t Preprocessor::Peek(Token* token)
{
    if(blockResult != EXPR_RESULT_NONE)
    {
        //if we are here this means we are after #if, #elif 
        //we need to consum new line
        // block is skipped if either expr is evaluated to 0 or block in given if-elif-else 
        // sequnce has already been included 
        if( conditionalBlocks.top().doneIncluding || blockResult == EXPR_RESULT_FALSE)
        {
            Token info;
            SkipTokensInBlock(&info);
        }
        else
        {
            conditionalBlocks.top().doneIncluding = true;
        }

        blockResult = EXPR_RESULT_NONE;
    }

fetch_token:
    do
    {
        *token = GetCurrToken();
        ConsumeToken();
    } while (token->type == TokenType::comment);
    
    if(token->type == TokenType::hash)
    {
        if(stages.ConstantExpr > 0)
        {
            IssueWarning(token, "preprocessor directive is not allowed inside constant expression");
            exit(-1);
        }

        int32_t ret = ExecuteDirective(token);
        if(ret != 0)
        {
            return ret;
        }
        goto fetch_token;
    }
    else if ( stages.ConstantExpr == 0 && 
              token->type >= TokenType::pp_include && 
              token->type <= TokenType::pp_defined)
    {
        // preprocessor specific types outside of preprocessor directives
        // are to be treated as identifiers
        token->type = TokenType::identifier;
    }
    
    else if(token->type == TokenType::eof && lexer.files.size() > 1)
    {
        lexer.PopFile();
        goto fetch_token;
    }
    // check for macros and special tokens
    else if(token->type == TokenType::identifier)
    {
        if(token->PossiblyErronous)
        {
            IssueWarning(token, "Errounsous identifier [%s]",
                     GetViewForToken(*token));
        }
        Macro *macro;
        std::string_view macroView = GetViewForToken(*token);
        bool hasEntry = FetchMacro(&macroView, &macro);
        if(!hasEntry && stages.ConstantExpr == 0)
        {
            // if we are not inside directive of any sort just return
            return 0;
        }
        FillQueueWithMacro(macro);
        goto fetch_token;
    }
    else if(token->type == TokenType::pp_defined)
    {
        *token = {};
        token->type = TokenType::numeric_constant;
        token->isDec = 1;
        if(ProcessDefined())
        {
            token->location = GetOneLocation();
        }
        else
        {
            token->location = GetZeroLocation();
        }
    }

    return 0;
}

void Preprocessor::ExecuteConstantExpr(Ast::Node *expr)
{ 
    Typed::Number constExpr = ExecuteNode(expr);
    stages.ConstantExpr = 0;
    if(constExpr.type == Typed::d_int64_t)
    {
        blockResult = constExpr.int64 == 0 ? EXPR_RESULT_FALSE : EXPR_RESULT_TRUE;
    }

}

Typed::Number Preprocessor::ExecuteNode(Ast::Node *expr)
{
    return ex.ExecuteNode(expr);
}

Token Preprocessor::StringifyParam(const std::vector<Token> &macroParam)
{
    std::string stringify;
    stringify.reserve(100);
    stringify += "\"";
    for(const Token& tok : macroParam)
    {
        if(!IsTokenOneOf(&tok, TokenType::new_line, TokenType::comment))
        {
            stringify += GetViewForToken(tok);
        }
    }

    stringify += "\"";

    int64_t tokenStringStart = fileOffset;
    WriteToPreprocessorFile(stringify.data(), stringify.length());
    int64_t tokenStringEnd = fileOffset;

    std::vector<Token> stringifyTok = lexer.LexFile(preprocessorFile, tokenStringStart, tokenStringEnd - tokenStringStart);
    assert(stringifyTok.size() == 1);
    return stringifyTok[0];
}

bool Preprocessor::FetchMacro(const std::string_view macroName, Macro **macro)
{
    return FetchMacro(&macroName, macro);
}

bool Preprocessor::FetchMacro(const std::string_view *macroName, Macro **macro)
{
    auto hashEntry = macros.find(*macroName);
    if(hashEntry == macros.end())
    {
        if(macro) { *macro = nullptr;}
        return false;
    }

    if(macro) { *macro = &hashEntry->second;}
    return true;
}

void Preprocessor::PushInitFile()
{
    FILE_ID id;
    if(manager->TryLoadFile(".builtin_defines.hpp", 15, &id) == -1)
    {
        system("echo | gcc -std=c99 -dM -E - > .builtin_defines.hpp");
        if(manager->TryLoadFile(".builtin_defines.hpp", 15, &id) == -1)
        {
            IssueWarning(nullptr, "Could not create .builtin_defines.hpp");
            exit(-1);
        }
    }

    lexer.PushFile(id, -1, -1);
}

void Preprocessor::FillQueueWithMacro(const Macro* macro)
{
    using TokenSequence = std::vector<Token>;
    if(stages.ConstantExpr == 1 && (macro == nullptr || macro->tokenList.size() == 0))
    {
        Token tokenConst = {};
        tokenConst.type = TokenType::numeric_constant;
        tokenConst.isHex = 1;
        tokenConst.location = macro == nullptr ? GetZeroLocation() : GetOneLocation();
        tokenQueue.push_front(tokenConst);
        return;
    }
  
    if(macro == nullptr)
    {
        return;
    }
    
    if(macro->flags.variadicArgs)
    {
        printf("Currently calling variadic args is not supported \n");
        exit(-1);
    }

    std::vector<TokenSequence> args;
    TokenSequence seq;
    if(macro->flags.callable)
    {
        ConsumeExpectedToken(TokenType::l_parentheses);
scan_arg:
        seq.clear();
        Token token = GetCurrToken();
        int nest = 0;
        while (!IsTokenOneOf(&token, TokenType::comma, TokenType::r_parentheses) || nest > 0)
        {
            if(token.type == TokenType::l_parentheses)
            {
                nest++;
            }
            else if(token.type == TokenType::r_parentheses)
            {
                nest--;
            }
            seq.push_back(token);
            ConsumeToken();
            token = GetCurrToken();
        }

        if(seq.size() == 0)
        {
            IssueWarning(&token, "Macro call has unfilled argument");
            exit(-1);
        }

        args.push_back(seq);
        if(token.type ==  TokenType::comma)
        {
            ConsumeToken();
            goto scan_arg;
        }
        ConsumeExpectedToken(TokenType::r_parentheses);
    }

    InsertMacroTokensIntoQueue(macro->tokenList, args, macro->argPlacement);
}   

uint8_t Preprocessor::GetTokenMode(const Token &token)
{
    if(token.isDec){ return MODE_DEC;}
    else if(token.isHex){ return MODE_HEX;}
    else if(token.isOct){ return MODE_OCT;}
    return MODE_BIN;
}

const char *Preprocessor::GetDataPtr(const Token *token)
{
    FILE_STATE state;
    manager->GetFileState(&token->location.id, &state);
    
    return state.fileData + token->location.offset;
}

int32_t Preprocessor::ExecuteDirective(Token *token)
{
    Token ppToken = GetCurrToken();
    
    /*
        some identifiers like 'define', 'error' have no special meaning in 
        non preprocessor context so that can be used freely.
        They get special meaning only if used inside #, so check for this and swap
        type aprioprietly
    */
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

void Preprocessor::IssueWarning(const Token *token, const char *errMsg, ...)
{
    va_list args;
    va_start(args, errMsg);
    if(token) {IssueWarning(&token->location.id, &token->location, errMsg, args);}
    else{IssueWarning(nullptr, nullptr, errMsg, args);}
    va_end(args);

    return;
}

void Preprocessor::IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char *errMsg, va_list args)
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
    printf(" Preprocessor warning\n");

    if(errMsg)
    {
        vprintf(errMsg, args);
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
get_token:
    if(tokenQueue.empty())
    {
        Token token;
        lexer.Lex(&token);
        tokenQueue.push_back(token);
    }

    if(tokenQueue.front().type == TokenType::eof && stages.eofTriggered == 1)
    {
        stages.eofTriggered = 0;
        tokenQueue.clear();
        lexer.PopFile();
        goto get_token;
    }
    else if(tokenQueue.front().type == TokenType::eof )
    {
        stages.eofTriggered = 1;
    }
    else if(tokenQueue.front().type == TokenType::comment )
    {
        tokenQueue.pop_front();
        goto get_token;
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

TokenType::Type Preprocessor::GetPreprocessorType(const Token *token)
{
    std::string_view tokenName = GetViewForToken(*token);
    if(tokenName == "include") { return TokenType::pp_include; }
    if(tokenName == "define") { return TokenType::pp_define; }
    if(tokenName == "ifdef") { return TokenType::pp_ifdef; }
    if(tokenName == "ifndef") { return TokenType::pp_ifndef; }
    if(tokenName == "elif") { return TokenType::pp_elif; }
    if(tokenName == "endif") { return TokenType::pp_endif; }
    if(tokenName == "line") { return TokenType::pp_line; }
    if(tokenName == "error") { return TokenType::pp_error; }
    if(tokenName == "pragma") { return TokenType::pp_pragma; }
    if(tokenName == "undef") { return TokenType::pp_undef; }
    if(tokenName == "defined") { return TokenType::pp_defined; }
    IssueWarning(token, "This identifier is not allowed to appear after #");
    exit(-1);
    return TokenType::none;
}

void Preprocessor::ConsumeExpectedToken(TokenType::Type type)
{
    Token token = GetCurrToken();
    ConsumeToken();
    if(token.type != type)
    {
        IssueWarning(&token, 
        "Given token is [%s] but expected is [%s]", 
        TokenType::tokenStr(token.type), TokenType::tokenStr(type));
        exit(-1);
    }
    return;
}

void Preprocessor::WriteToPreprocessorFile(const char *data, int64_t dataLen)
{
    if(manager->WriteToFile(data, fileOffset, dataLen ,&preprocessorFile) != 0)
    {
        printf("Run out of memory for preprocessor file \n");
        exit(-1);
    }

    fileOffset += dataLen;
}

void Preprocessor::InsertMacroTokensIntoQueue(
    const std::vector<Token>& macroTokens, 
    const std::vector<std::vector<Token>>& args,
    const std::vector<MacroArgPlacement>& argPlacement)
{
    using TokenSequence = std::vector<Token>;
    size_t n = tokenQueue.size();
    size_t argOffsets = 0;

    bool concatTokens = false;
    size_t insertionOffset = 0;
    for(size_t i =0; i < macroTokens.size(); i++)
    {
        if(argPlacement.size() > 0 &&
           argPlacement[argOffsets].argPos == i )
        {
            const TokenSequence& argTokens =  args[argPlacement[argOffsets].argId];
            for(size_t j =0; j <argTokens.size(); j++)
            {
                tokenQueue.push_front(argTokens[j]);
            }
            argOffsets++;
        }
        else
        {
            if(IsTokenOneOf(&macroTokens[i],  TokenType::ellipsis))
            {
                printf("ellipsis are not supported \n");
                exit(-1);
            }
            
            if(macroTokens[i].type == TokenType::hash)
            {   
                const MacroArgPlacement& argPlcm = argPlacement[argOffsets];
                argOffsets++;
                if(argPlcm.argPos != i + 1)
                {
                    IssueWarning(&macroTokens[i], "stringify token should be followed by parameter");
                    exit(-1);
                }
                Token stringify = StringifyParam(args[argPlcm.argId]);
                tokenQueue.push_front(stringify);
                i+=1;
                continue;
            }
            else if(macroTokens[i].type == TokenType::d_hash)
            {
                concatTokens = true;
                insertionOffset = tokenQueue.size();
                continue;
            }

            tokenQueue.push_front(macroTokens[i]);
        }

        if(concatTokens)
        {
            concatTokens = false;
            size_t tokenL = tokenQueue.size() - insertionOffset;
            if(tokenL == 0)
            {
                IssueWarning(&tokenQueue.front(), "## requires token on both left and right side");
                exit(-1);
            }
            size_t tokenR = tokenL - 1;
            std::vector<Token> tokens = MergeTokensInLexer(&tokenQueue.at(tokenL), &tokenQueue.at(tokenR));
            tokenQueue.insert(tokenQueue.begin() + tokenL + 1, tokens.rbegin(), tokens.rend());
            tokenQueue.erase(tokenQueue.begin() + tokenR, tokenQueue.begin() + tokenL + 1);
    
            // top of lexer stack has newly created token, 
            // read all of them before proceding any further
            
        }
        
        // as per C 99 6.10.3.3  ## is run before #
    }

    std::reverse(tokenQueue.begin(),tokenQueue.end() - n);
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
    stages.ConstantExpr = 1;
    ConditionalBlock block = CreateBlock();
    conditionalBlocks.push(block);
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
            IssueWarning(&info, "#else block may only end with #endif\n" );
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
        IssueWarning(&headerToken, "Incorrect file include format");
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
    lexer.PushFile(headerFileId, -1, -1);
    return 0;
}

int32_t Preprocessor::HandleDefine()
{
    Token defineIdentifier = GetCurrToken();
    ConsumeExpectedToken(TokenType::identifier);

    Macro macro = {};
    std::string_view macroName = GetViewForToken(defineIdentifier);
    Token macroExpansion = GetCurrToken();
    // key - argname ; value - argument number
    std::unordered_map<std::string_view, uint16_t> argNames;

    // check if macro is callable
    if(macroExpansion.type == TokenType::l_parentheses &&
                 macroExpansion.skippedHorizWhitespace == 0)
    {
        macro.flags.callable = 1;
        ConsumeExpectedToken(TokenType::l_parentheses);
        Token token = GetCurrToken();
        size_t argIt = 0;
        while (token.type == TokenType::identifier)
        {
            ConsumeToken();
            std::string_view argName(GetViewForToken(token));
            // insert arg into map
            auto nameIter = argNames.find(argName);
            if(nameIter != argNames.end()) 
            {
                // terminate on macro error
                IssueWarning(&token, "Argument name repeated in macro\n");
                exit(-1);
            }

            argNames[argName] = argIt;
            token = GetCurrToken();
            if(token.type == TokenType::r_parentheses)
            {
                ConsumeToken();
                macroExpansion = GetCurrToken();
                break;
            }
            ConsumeExpectedToken(TokenType::comma);
            token = GetCurrToken();
            argIt++;
        }
        if(token.type == TokenType::ellipsis)
        {
            macro.flags.variadicArgs = 1;
            ConsumeExpectedToken(TokenType::ellipsis);
            ConsumeExpectedToken(TokenType::r_parentheses);
            macroExpansion = GetCurrToken();
        }
    }

    while (!IsTokenOneOf(&macroExpansion, TokenType::new_line, TokenType::eof))
    {
        if(macroExpansion.type == TokenType::identifier)
        {
            std::string_view tokenName(GetViewForToken(macroExpansion));
            // insert arg into map
            auto nameIter = argNames.find(tokenName);
            if(nameIter != argNames.end()) 
            {
                macro.argPlacement.emplace_back(nameIter->second, macro.tokenList.size());
            }
        }

        if(macroExpansion.type != TokenType::line_splice)
        {
            macro.tokenList.push_back(macroExpansion);
        }
        ConsumeToken();
        macroExpansion = GetCurrToken();
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
    else
    {
        stages.ConstantExpr = 1;
    }

    return 0;
}

int32_t Preprocessor::HandleEndif()
{
    Token token =  GetCurrToken(); // token used in logging only

    ConsumeExpectedToken(TokenType::new_line);
    if(conditionalBlocks.empty())
    {
        IssueWarning(&token, "#endif used without #if");
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

int32_t Preprocessor::HandleDefined()
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
            IssueWarning(&tokenWarning, msg);
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
    IssueWarning(&token, "Block is not enclosed with #endif\n");
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

SourceLocation Preprocessor::GetZeroLocation()
{
    SourceLocation loc = {};
    loc.id = preprocessorFile;
    loc.offset = 0;
    loc.len = 1;
    loc.line = 0;
    return loc;
}

SourceLocation Preprocessor::GetOneLocation()
{
    SourceLocation loc = {};
    loc.id = preprocessorFile;
    loc.offset = 1;
    loc.len = 1;
    loc.line = 0;
    return loc;
}

std::vector<Token> Preprocessor::MergeTokensInLexer(const Token *left, const Token *right)
{
    std::string_view leftToken = GetViewForToken(*left);
    std::string_view rightToken = GetViewForToken(*right);
    int64_t tokenStringStart = fileOffset;
    WriteToPreprocessorFile(leftToken.data(), leftToken.length());
    WriteToPreprocessorFile(rightToken.data(), rightToken.length());
    int64_t tokenStringEnd = fileOffset;

    return lexer.LexFile(preprocessorFile, tokenStringStart, tokenStringEnd - tokenStringStart);
}

bool Preprocessor::ProcessDefined()
{
    Token token = GetCurrToken();
    bool r_paren = false;
    bool isDefined = false;
    if(token.type == TokenType::l_parentheses)
    {
        ConsumeExpectedToken(TokenType::l_parentheses);
        token = GetCurrToken();
        r_paren = true;
    }

    ConsumeExpectedToken(TokenType::identifier);

    auto hashEntry = macros.find(GetViewForToken(token));
    if(hashEntry == macros.end()){isDefined = false;}
    else {isDefined = true;}

    if(r_paren)
    {
        ConsumeExpectedToken(TokenType::r_parentheses);
    }
    return isDefined;
}
