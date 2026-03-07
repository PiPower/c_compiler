#pragma once
#include "AstNode.hpp"
#include "../utils/FileManager.hpp"
#include "Preprocessor.hpp"

struct Parser
{
    Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    void Parse();
private:
    // expression parsing
    Ast::Node* ParseConstantExpr();
    Ast::Node* ParseExpression();
    Ast::Node* AssignmentExpression();
    Ast::Node* ConditionalExpression();
    Ast::Node* LogicalOrExpression();
    Ast::Node* LogicalAndExpression();
    Ast::Node* InclusiveOrExpression();
    Ast::Node* ExclusiveOrExpression();
    Ast::Node* AndExpression();
    Ast::Node* EqualityExpression();
    Ast::Node* RelationalExpression();
    Ast::Node* ShiftExpression();
    Ast::Node* AdditiveExpression();
    Ast::Node* MultiplicativeExpression();
    Ast::Node* CastExpression();
    Ast::Node* ParseCastingType();
    Ast::Node* UnaryExpression();
    Ast::Node* PostfixExpression();
    Ast::Node* PrimaryExpression();
public:
    Preprocessor PP;
    const CompilationOpts* opts;

    PagedBuffer nodeBuffer;
};
