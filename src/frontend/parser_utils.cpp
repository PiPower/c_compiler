#include "../../include/frontend/parser_utils.hpp"

NodeDataType transformToDataType(const std::vector<Token>& declSpec)
{
    if(declSpec.size() == 1 && declSpec[0].type == TokenType::INT) return NodeDataType::INT_64;
    else if(declSpec.size() > 0){
        fprintf(stdout, "unsupported declaration spec");
        exit(-1);
    }
    return NodeDataType::NONE;
}

bool isTypeSpecifier(Token token)
{
    return (int)token.type >= (int)TokenType::VOID && (int)token.type <= (int)TokenType::_COMPLEX  ;
}


NodeType tokenMathTypeToNodeType(const Token token)
{
    switch (token.type)
    {
    case TokenType::PLUS :
        return NodeType::ADD;
    case TokenType::MINUS :
        return NodeType::SUBTRACT;
    case TokenType::SLASH :
        return NodeType::DIVIDE;
    case TokenType::PERCENT :
        return NodeType::DIVIDE_MODULO;
    case TokenType::STAR :
        return NodeType::MULTIPLY;
    case TokenType::L_SHIFT :
        return NodeType::L_SHIFT;
    case TokenType::R_SHIFT :
        return NodeType::R_SHIFT;
    case TokenType::LESS :
        return NodeType::LESS;
    case TokenType::GREATER :
        return NodeType::GREATER;
    case TokenType::LESS_EQUAL :
        return NodeType::LESS_EQUAL;
    case TokenType::GREATER_EQUAL :
        return NodeType::GREATER_EQUAL;
    case TokenType::EQUAL_EQUAL :
        return NodeType::EQUAL;
    case TokenType::BANG_EQUAL :
        return NodeType::NOT_EQUAL;
    case TokenType::PIPE :
        return NodeType::OR;
    case TokenType::AMPRESAND :
        return NodeType::AND;
    case TokenType::CARET :
        return NodeType::EXC_OR;
    case TokenType::DOUBLE_AMPRESAND :
        return NodeType::LOG_AND;
    case TokenType::DOUBLE_PIPE :
        return NodeType::LOG_OR;
    case TokenType::COMMA:
        return NodeType::EXPR_GLUE;
    }

    fprintf(stdout, "unexpected token at line %d \n", token.line);
    exit(-1);
}
NodeType assignementTokenToNodeType(const Token token)
{
    return (NodeType) ((int)token.type - (int)TokenType::EQUAL + (int)NodeType::ASSIGNMENT) ;
}
