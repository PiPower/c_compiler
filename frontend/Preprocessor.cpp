#include "Preprocessor.hpp"
#include <cassert>
#include <string.h>
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <functional>

static const char* PreprocessorFlename = "preprocessor_file.comp";
static const char* InsertableValues = "01";

template<typename _Tp>
struct right_shift 
{
    _GLIBCXX14_CONSTEXPR
    _Tp
    operator()(const _Tp& __x, const _Tp& __y) const
    { return __x >> __y; }
};

template<typename _Tp>
struct left_shift
{
    _GLIBCXX14_CONSTEXPR
    _Tp
    operator()(const _Tp& __x, const _Tp& __y) const
    { return __x << __y; }
};

template<typename Op>
Typed::Number BinaryOp(Preprocessor* pp,  Ast::Node* node)
{
    Typed::Number l = pp->ExecuteNode(node->lChild);
    Typed::Number r = pp->ExecuteNode(node->rChild);

    Typed::Number out{};
    out.int64 = Op{}(l.int64, r.int64);
    out.type = Typed::d_int64_t;

    return out;
}


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

Preprocessor::Preprocessor(FILE_STATE mainFile, FileManager *manager, const CompilationOpts* opts)
:
lexer(mainFile, manager, opts), manager(manager), opts(opts), stages({}), blockResult(EXPR_RESULT_NONE)
{
    assert(opts != nullptr);
    constexpr size_t initiialBufferSize = 500;
    constantNodes.reserve(initiialBufferSize);
    manager->CreateInternalFile(PreprocessorFlename, 
        strlen(PreprocessorFlename), InsertableValues, 2, &preprocessorFile);

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
    // check for macros and special tokens
    else if(token->type == TokenType::identifier)
    {
        auto hashEntry = macros.find(GetViewForToken(*token));
        if(hashEntry == macros.end() && stages.If == 0)
        {
            // if we are not inside directive of any sort just return
            return 0;
        }
        FillQueueWithMacro(hashEntry);
        *token = GetCurrToken();
        ConsumeToken();
        return 0;
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
    stages.If = 0;
    if(constExpr.type == Typed::d_int64_t)
    {
        blockResult = constExpr.int64 == 0 ? EXPR_RESULT_FALSE : EXPR_RESULT_TRUE;
    }

}

Typed::Number Preprocessor::ExecuteNode(Ast::Node *expr)
{
    Typed::Number numOut;
    numOut.type = Typed::d_int64_t;
    switch (expr->type)
    {
    case Ast::NodeType::constant:
        if(expr->token.isFloat) 
        {
           IssueWarning(&expr->token, "Expression must have integral type");
           exit(-1);
        }
        numOut.int64 = stringToInt64(GetDataPtr(&expr->token), 
                expr->token.location.len, GetTokenMode(expr->token));
        return numOut;
    case Ast::NodeType::string_literal:
        IssueWarning(&expr->token, "Expression must have integral type");
        exit(-1);
        break;
    case Ast::NodeType::character:
        if(expr->token.location.len > 3)
        {
            IssueWarning(&expr->token, "Multi byte character constants  are not supported\n");
            exit(-1);
        }
        numOut.int64 = stringToChar(GetDataPtr(&expr->token), expr->token.location.len);
        return numOut;
    case Ast::NodeType::op_log_negate:
        numOut = ExecuteNode(expr->lChild);
        numOut.int64 = numOut.int64 == 0;
        return numOut;
    // binary ops
    case Ast::NodeType::op_less_equal: return BinaryOp<std::less_equal<int64_t>>(this, expr);
    case Ast::NodeType::op_less: return BinaryOp<std::less<int64_t>>(this, expr);
    case Ast::NodeType::op_greater_equal: return BinaryOp<std::greater_equal<int64_t>>(this, expr);
    case Ast::NodeType::op_greater: return BinaryOp<std::greater<int64_t>>(this, expr);
    case Ast::NodeType::op_l_shift: return BinaryOp<left_shift<int64_t>>(this, expr);
    case Ast::NodeType::op_r_shift: return BinaryOp<right_shift<int64_t>>(this, expr);
    case Ast::NodeType::op_inc_or: return BinaryOp<std::bit_or<int64_t>>(this, expr);
    case Ast::NodeType::op_exc_or: return BinaryOp<std::bit_xor<int64_t>>(this, expr);
    case Ast::NodeType::op_and: return BinaryOp<std::bit_and<int64_t>>(this, expr);
    case Ast::NodeType::op_log_and: return BinaryOp<std::logical_and<int64_t>>(this, expr);
    case Ast::NodeType::op_log_or: return BinaryOp<std::logical_or<int64_t>>(this, expr);
    case Ast::NodeType::op_divide: return BinaryOp<std::divides<int64_t>>(this, expr);
    case Ast::NodeType::op_divide_modulo: return BinaryOp<std::modulus<int64_t>>(this, expr);
    case Ast::NodeType::op_subtract: return BinaryOp<std::minus<int64_t>>(this, expr);
    case Ast::NodeType::op_add: return BinaryOp<std::plus<int64_t>>(this, expr);
    case Ast::NodeType::op_multiply: return BinaryOp<std::multiplies<int64_t>>(this, expr);
    case Ast::NodeType::op_equal: return BinaryOp<std::equal_to<int64_t>>(this, expr);
    case Ast::NodeType::expression: return ExecuteNode(expr->lChild);
    // forbiden element
    case Ast::NodeType::op_pre_inc:
    case Ast::NodeType::op_post_inc:
    case Ast::NodeType::op_pre_dec:
    case Ast::NodeType::op_post_dec:
    case Ast::NodeType::assignment:
    case Ast::NodeType::mul_assignment:
    case Ast::NodeType::div_assignment:
    case Ast::NodeType::mod_assignment:
    case Ast::NodeType::add_assignment:
    case Ast::NodeType::sub_assignment:
    case Ast::NodeType::l_shift_assignment:
    case Ast::NodeType::r_shift_assignment:
    case Ast::NodeType::and_assignment:
    case Ast::NodeType::exc_or_assignment:
    case Ast::NodeType::inc_or_assignment:
    case Ast::NodeType::function_call:
    case Ast::NodeType::args_expr_list:
    case Ast::NodeType::cast:
    case Ast::NodeType::get_addr:
    case Ast::NodeType::dref_ptr:
    case Ast::NodeType::array_access:
    case Ast::NodeType::struct_access:
    case Ast::NodeType::ptr_access:
    default:
        IssueWarning(&expr->token,
        "Operation [%s] is not allowed in preprocessing directive \n",
        Ast::nodeStr(expr->type));
        exit(-1);
        break;
    }
    return numOut;
}

void Preprocessor::StartConstantExpr()
{
    stages.ConstantExpr = 1;
}

void Preprocessor::StopConstantExpr()
{
    stages.ConstantExpr = 0;
}

void Preprocessor::FillQueueWithMacro(MacroMapIter& macroIter)
{
    size_t n = tokenQueue.size();

    if(macroIter == macros.end() || macroIter->second.tokenList.size() == 0)
    {
        Token tokenConst = {};
        tokenConst.type = TokenType::numeric_constant;
        tokenConst.isHex = 1;
        tokenConst.location = macroIter == macros.end() ? GetZeroLocation() : GetOneLocation();
        tokenQueue.push_front(tokenConst);
    }
    else
    {
        Macro* macro = &macroIter->second;
        for(const Token& token : macro->tokenList)
        {
            tokenQueue.push_front(token);
        }
    }
    
    std::deque<Token>::reverse_iterator lastElem = std::prev(tokenQueue.rend(), n);
    std::reverse(tokenQueue.rbegin(), lastElem);
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
    printf(" Preprocessor warning \n");

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
        IssueWarning(&token, 
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
    else
    {
        stages.If = 1;
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
