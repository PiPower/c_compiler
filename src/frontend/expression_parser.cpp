#include "../../include/frontend/parser.hpp"

#include <string>
using namespace std;
const vector<TokenType> assignmentType =  {TokenType::EQUAL, TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL, 
            TokenType::PERCENT_EQUAL, TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL, TokenType::L_SHIFT_EQUAL,
            TokenType::R_SHIFT_EQUAL, TokenType::AMPRESAND_EQUAL,TokenType::CARET_EQUAL, TokenType::PIPE_EQUAL};


typedef AstNode* (* parseFunctionPtr)(Scanner& scanner);
//parse loop for left to right op order ie x op yop z = ((x op y) op z)
static AstNode* parseLoop(Scanner& scanner, parseFunctionPtr parsingFunction, AstNode* root, vector<TokenType> types)
{
    while (scanner.currentTokenOneOf(types))
    {
        Token operatorToken = scanner.popToken();
        AstNode* parent = new AstNode{tokenMathTypeToNodeType(operatorToken), {}, NodeDataType::INFERED};
        AstNode* right = parsingFunction(scanner);
        parent->children.push_back(root);
        parent->children.push_back(right);

        root = parent;
    }

    return root;
}



static AstNode* primaryExpression(Scanner& scanner)
{
    Token token = scanner.popToken();
    AstNode* root;
    switch (token.type)
    {
    case TokenType::IDENTIFIER:
        root = new AstNode{NodeType::IDENTIFIER, {}, NodeDataType::NONE, new string()};
        *((string*)root->context.arbitraryData) = *(string*)token.context.data;
        return root;
    case TokenType::CONSTANT:
        root = new AstNode{NodeType::CONSTANT, {}, NodeDataType::INT_64};
        root->context.int_32 = token.context.int_32;
        return root;
    case TokenType::L_PARENTHESES:
        root = parseExpression(scanner);
        scanner.consume(TokenType::R_PARENTHESES);
        return root;
    default:
        fprintf(stdout, "Unexpected token for primary expression \n");
        exit(-1);
        break;
    }
}

static AstNode* parseArgsExpression(Scanner& scanner)
{
    AstNode* params = new AstNode{NodeType::FUNCTION_PARAMS,  {}, NodeDataType::NONE};
    while (true)
    {
        if(scanner.match(TokenType::R_PARENTHESES))
        {
            break;
        }
        AstNode* arg = assignmentExpression(scanner);
        params->children.push_back(arg);
        if(scanner.match(TokenType::R_PARENTHESES))
        {
            break;
        }
        scanner.consume(TokenType::COMMA);
    }
    return params;
}

static AstNode* postfixExpression(Scanner& scanner)
{
    AstNode* primary = primaryExpression(scanner);
    if(!scanner.match(TokenType::L_PARENTHESES))
    {
        return primary;
    }

    AstNode* args = parseArgsExpression(scanner);
    AstNode* call = new AstNode{NodeType::FUNCTION_CALL, {primary, args}, NodeDataType::INFERED};
    return call;
}


static AstNode* unaryExpression(Scanner& scanner)
{
    return postfixExpression(scanner);
}

static AstNode* castExpression(Scanner& scanner)
{
    return unaryExpression(scanner);
}

static AstNode* multiplicativeExpression(Scanner& scanner)
{
    AstNode* root = castExpression(scanner);
    return parseLoop(scanner, castExpression, root, {TokenType::STAR, TokenType::SLASH, TokenType::PERCENT});
}

static AstNode* additiveExpression(Scanner& scanner)
{
    AstNode* root = multiplicativeExpression(scanner);
    return parseLoop(scanner, multiplicativeExpression, root, {TokenType::PLUS, TokenType::MINUS});
}

static AstNode* shiftExpression(Scanner& scanner)
{
    AstNode* root = additiveExpression(scanner);
    return parseLoop(scanner, additiveExpression, root, {TokenType::L_SHIFT, TokenType::R_SHIFT});
}

static AstNode* relationalExpression(Scanner& scanner)
{
    AstNode* root = shiftExpression(scanner);
    return parseLoop(scanner, shiftExpression, root, {TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL});
}

static AstNode* equalityExpression(Scanner& scanner)
{
    AstNode* root = relationalExpression(scanner);
    return parseLoop(scanner, relationalExpression, root, {TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL});
}

static AstNode* andExpression(Scanner& scanner)
{
    AstNode* root = equalityExpression(scanner);
    return parseLoop(scanner, equalityExpression, root, {TokenType::AMPRESAND});
}

static AstNode* exc_orExpression(Scanner& scanner)
{
    AstNode* root = andExpression(scanner);
    return parseLoop(scanner, andExpression, root, {TokenType::CARET});
}

static AstNode* orExpression(Scanner& scanner)
{
    AstNode* root = exc_orExpression(scanner);
    return parseLoop(scanner, exc_orExpression, root, {TokenType::PIPE});
}

static AstNode* log_andExpression(Scanner& scanner)
{
    AstNode* root = orExpression(scanner);
    if(scanner.getCurrentToken().type != TokenType::DOUBLE_AMPRESAND)
    {
        return root;
    }
    
    AstNode* parent = new AstNode{NodeType::LOG_AND, {root}, NodeDataType::INFERED};
    while (scanner.match(TokenType::DOUBLE_AMPRESAND))
    {
        AstNode* right = orExpression(scanner);
        parent->children.push_back(right);
    }
    return parent;
}

static AstNode* log_orExpression(Scanner& scanner)
{
    AstNode* root = log_andExpression(scanner);
    if(scanner.getCurrentToken().type != TokenType::DOUBLE_PIPE)
    {
        return root;
    }

    AstNode* parent = new AstNode{NodeType::LOG_OR, {root}, NodeDataType::INFERED};
    while (scanner.match(TokenType::DOUBLE_PIPE))
    {
        AstNode* right = log_andExpression(scanner);
        parent->children.push_back(right);
    }
    
    return parent;
}

static AstNode* conditionalExpression(Scanner& scanner)
{
    AstNode* root = log_orExpression(scanner);
    if(scanner.getCurrentToken().type != TokenType::QUESTION_MARK)
    {
        return root;
    }
    scanner.incrementTokenId();
    AstNode* expr = parseExpression(scanner);
    scanner.consume(TokenType::COLON);
    AstNode* cond_expr = conditionalExpression(scanner);

    AstNode* op = new AstNode{NodeType::CONDITIONAL_EXPRESSION, {root, expr, cond_expr}, NodeDataType::INFERED};
    return op;
}

// assignment is right to left so we cannot use parseLoop
AstNode* assignmentExpression(Scanner& scanner)
{
    AstNode* root = conditionalExpression(scanner);
    if (!scanner.currentTokenOneOf(assignmentType))
    {
        return root;
    }
    Token operatorToken = scanner.popToken();
    AstNode* parent = new AstNode{assignementTokenToNodeType(operatorToken), {}, NodeDataType::INFERED};
    AstNode* child = assignmentExpression(scanner);
    if(parent->nodeType == NodeType::ASSIGNMENT)
    {
        parent->children.push_back(root);
        parent->children.push_back(child);
        return parent;
    }   
    //desugaring
    NodeType opType = (NodeType)( ((int)parent->nodeType - (int)NodeType::ADD_ASSIGNMENT) + (int)NodeType::ADD );

    AstNode* opNode = new AstNode{opType, {root, child}, NodeDataType::INFERED};
    parent->nodeType = NodeType::ASSIGNMENT;
    parent->children.push_back(root);
    parent->children.push_back(opNode);
    return parent;
}

AstNode* parseExpression(Scanner& scanner)
{
    AstNode* root = assignmentExpression(scanner);
    return parseLoop(scanner, assignmentExpression, root, {TokenType::COMMA});
}

AstNode* parseConstantExpression(Scanner& scanner)
{
    return conditionalExpression(scanner);
}