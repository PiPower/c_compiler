#include "Parser.hpp"
#include <cassert>
Parser::Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
PP(mainFile, manager, opts), opts(opts)
{
    assert(opts != nullptr);
}

void Parser::Parse()
{
start_parsing:
    Token token;
    PP.Peek(&token);

    if(PP.stages.If > 0)
    {
        Ast::Node* expr = ParseConstantExpr();
        PP.ExecuteConstantExpr(expr);
        goto start_parsing;
    }

    while (token.type != TokenType::eof)
    {
        PP.Peek(&token);
    }


    
}

Ast::Node *Parser::ParseConstantExpr()
{
    return ConditionalExpression();
}

Ast::Node* Parser::ParseExpression()
{
    return AssignmentExpression();
}

Ast::Node* Parser::PrimaryExpression()
{
    return nullptr;
}

Ast::Node* Parser::PostfixExpression()
{
    return nullptr;
}

Ast::Node* Parser::UnaryExpression()
{
    return nullptr;
}

Ast::Node* Parser::CastExpression()
{
    return nullptr;
}

Ast::Node* Parser::MultiplicativeExpression()
{
    return nullptr;
}

Ast::Node* Parser::AdditiveExpression()
{
    return nullptr;
}

Ast::Node* Parser::ShiftExpression()
{
    return nullptr;
}

Ast::Node* Parser::RelationalExpression()
{
    return nullptr;
}

Ast::Node* Parser::EqualityExpression()
{
    return nullptr;
}

Ast::Node* Parser::AndExpression()
{
    return nullptr;
}

Ast::Node* Parser::ExclusiveOrExpression()
{
    return nullptr;
}

Ast::Node* Parser::InclusiveOrExpression()
{
    return nullptr;
}

Ast::Node* Parser::LogicalAndExpression()
{
    return nullptr;
}

Ast::Node* Parser::LogicalOrExpression()
{
    return nullptr;
}

Ast::Node* Parser::ConditionalExpression()
{
    return nullptr;
}

Ast::Node* Parser::AssignmentExpression()
{
    return nullptr;
}


