#include "Preprocessor.hpp"
#include <cassert>

Preprocessor::Preprocessor(FILE_STATE mainFile, FileManager *manager, const CompilationOpts* opts)
:
lexer(mainFile, manager, opts), opts(opts)
{
    assert(opts != nullptr);
}

int32_t Preprocessor::Peek(Token* token)
{
    int32_t ret = 0;
    do
    {
        ret = lexer.Lex(token);
    } while (token->type == TokenType::comment || token->type == TokenType::new_line);
    
    if(token->type == TokenType::hash)
    {
        ret = ExecuteDirective(token);
    }

    return 0;
}

int32_t Preprocessor::ExecuteDirective(Token *token)
{
    Token ppToken;
    lexer.Lex(&ppToken);
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

int32_t Preprocessor::HandleIf()
{
    return 0;
}

int32_t Preprocessor::HandleElse()
{
    return 0;
}

int32_t Preprocessor::HandleInclude()
{
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