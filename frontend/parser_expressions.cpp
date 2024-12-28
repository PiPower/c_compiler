#include "parser_internal.hpp"
#include "parser_utils.hpp"
#include <stack>

using namespace std;

static constexpr TokenType assignmentTypes[] = { 
    TokenType::EQUAL, TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL, 
    TokenType::PERCENT_EQUAL, TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL, 
    TokenType::L_SHIFT_EQUAL, TokenType::R_SHIFT_EQUAL, 
    TokenType::AMPRESAND_EQUAL,TokenType::CARET_EQUAL, 
    TokenType::PIPE_EQUAL
    };

static constexpr TokenType unaryOperators[] = {
    TokenType::PLUS_PLUS,TokenType::MINUS_MINUS,
    TokenType::BANG, TokenType::TILDE, TokenType::PLUS
    };

static constexpr TokenType mulTypes[] = {
    TokenType::STAR, TokenType::SLASH, TokenType::PERCENT
    };

static constexpr TokenType additiveTypes[] =  {
    TokenType::PLUS, TokenType::MINUS
    };

static constexpr TokenType shiftTypes[] =  {
    TokenType::L_SHIFT, TokenType::R_SHIFT
    };

static constexpr TokenType relTypes[] =  {
    TokenType::GREATER, TokenType::GREATER_EQUAL, 
    TokenType::LESS, TokenType::LESS_EQUAL
    };

static constexpr TokenType eqTypes[] =  {
    TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL
    };

    static constexpr TokenType andTypes[] = {
    TokenType::AMPRESAND
    };

static constexpr TokenType escOrTypes[] =  {
    TokenType::CARET
    };

static constexpr TokenType incOrTypes[] =  {
    TokenType::PIPE
    };

static constexpr TokenType logAndTypes[] =  {
    TokenType::DOUBLE_AMPRESAND
    };

static constexpr TokenType logOrTypes[] =  {
    TokenType::DOUBLE_PIPE
    };


AstNode *parseExpression(ParserState *parser)
{
    return assignmentExpression(parser);
}

AstNode *assignmentExpression(ParserState *parser)
{
    AstNode* condExpr;
    stack<AstNode*> nodes;
    stack<NodeType> operators;
    while (true)
    {    
        if(setjmp(parser->assignmentJmp) == 0 )
        {
            parser->isParsingAssignment = true;
            condExpr = conditionalExpression(parser);

            break;
        }
        else
        {
            nodes.push(parser->jmpHolder);
            parser->jmpHolder = nullptr;
            operators.push(assignementTokenToNodeType(GET_TOKEN(parser)) );
        }
    }

    AstNode* root = condExpr;
    while (operators.size() > 0)
    {
        AstNode* assignement = ALLOCATE_NODE(parser);
        assignement->nodeType = operators.top();
        assignement->children.push_back(root);
        assignement->children.push_back(nodes.top() );

        operators.pop();
        nodes.pop();
        root = assignement;
    }
    
    return root;
}

AstNode *conditionalExpression(ParserState *parser)
{
    AstNode* logOr = logOrExpression(parser);
    if(PEEK_TOKEN(parser).type != TokenType::QUESTION_MARK)
    {
        return logOr;
    }
    CONSUME_TOKEN(parser, TokenType::QUESTION_MARK);
    AstNode* expr = parseExpression(parser);
    CONSUME_TOKEN(parser, TokenType::COLON);
    AstNode* condExpr = conditionalExpression(parser);

    AstNode* root = ALLOCATE_NODE(parser);
    root->nodeType = NodeType::CONDITIONAL_EXPRESSION;
    root->children.push_back(logOr);
    root->children.push_back(expr);
    root->children.push_back(condExpr);
    return root;

}

AstNode *logOrExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(logOrTypes)/sizeof(TokenType);
    AstNode* root = logAndExpression(parser);
    return parseLoop(parser, logAndExpression, root, logOrTypes, len);
}

AstNode *logAndExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(logAndTypes)/sizeof(TokenType);
    AstNode* root = incOrExpression(parser);
    return parseLoop(parser, incOrExpression, root, logAndTypes, len);
}

AstNode *incOrExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(incOrTypes)/sizeof(TokenType);
    AstNode* root = excOrExpression(parser);
    return parseLoop(parser, excOrExpression, root, incOrTypes, len);
}

AstNode *excOrExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(escOrTypes)/sizeof(TokenType);
    AstNode* root = AndExpression(parser);
    return parseLoop(parser, AndExpression, root, escOrTypes, len);
}

AstNode *AndExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(andTypes)/sizeof(TokenType);
    AstNode* root = equalityExpression(parser);
    return parseLoop(parser, equalityExpression, root, andTypes, len);
}

AstNode *equalityExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(eqTypes)/sizeof(TokenType);
    AstNode* root = relationalExpression(parser);
    return parseLoop(parser, relationalExpression, root, eqTypes, len);
}

AstNode *relationalExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(relTypes)/sizeof(TokenType);
    AstNode* root = shiftExpression(parser);
    return parseLoop(parser, shiftExpression, root, relTypes, len);
}

AstNode *shiftExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(shiftTypes)/sizeof(TokenType);
    AstNode* root = additiveExpression(parser);
    return parseLoop(parser, additiveExpression, root, shiftTypes, len);
}

AstNode *additiveExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(additiveTypes)/sizeof(TokenType);
    AstNode* root = multiplicativeExpression(parser);
    return parseLoop(parser, multiplicativeExpression, root, additiveTypes, len);
}

AstNode *multiplicativeExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(mulTypes)/sizeof(TokenType);
    AstNode* root = castExpression(parser);
    return parseLoop(parser, castExpression, root, mulTypes, len);
}

AstNode *castExpression(ParserState *parser)
{
    if(parser->isParsingAssignment)
    {
        static constexpr uint64_t len = sizeof(assignmentTypes)/sizeof(TokenType);

        parser->isParsingAssignment = false;
        // assignmentJmp might be overwrittent by recursive
        // calls so its saved here
        jmp_buf buff ;
        buff[0] = parser->assignmentJmp[0];

        AstNode* root = unaryExpression(parser);
        if(!parser->scanner->currentTokenOneOf(assignmentTypes, len))
        {
            return root;
        }
        parser->jmpHolder = root;
        longjmp(buff, 1);
    }
    return unaryExpression(parser);
}

AstNode *unaryExpression(ParserState *parser)
{
    static constexpr uint64_t len = sizeof(unaryOperators)/sizeof(TokenType);
    // no unary 
    if(!parser->scanner->currentTokenOneOf(unaryOperators, len))
    {
        return postfixExpression(parser);
    }

    Token token = parser->scanner->getToken();
    if(token.type == TokenType::PLUS_PLUS || token.type == TokenType::MINUS_MINUS)
    {
        AstNode* root = ALLOCATE_NODE(parser);
        root->nodeType = token.type == TokenType::PLUS ? NodeType::PRE_INC : NodeType::PRE_DEC;
        root->children.push_back(unaryExpression(parser));
        return root;
    }
    AstNode* root = ALLOCATE_NODE(parser);
    root->nodeType = token.type == TokenType::BANG ? NodeType::LOG_NOT : NodeType::COMPLIMENT;
    root->nodeType = token.type == TokenType::PLUS ? NodeType::ADD : root->nodeType;
    root->children.push_back(castExpression(parser));
    return root;
}

AstNode *postfixExpression(ParserState *parser)
{
    AstNode* primaryExpr = primaryExpression(parser);
    if(!TOKEN_MATCH(parser, TokenType::L_PARENTHESES))
    {
        return primaryExpr;
    }
    AstNode* fnCall = ALLOCATE_NODE(parser);
    AstNode* args = parseArgExprList(parser);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    fnCall->nodeType = NodeType::FUNCTION_CALL;
    fnCall->children = {primaryExpr, args};
    return fnCall;  
}

AstNode *primaryExpression(ParserState *parser)
{
    Token token = parser->scanner->getToken();
    AstNode* root;
    switch (token.type)
    {
    case TokenType::IDENTIFIER:
    {
        root = ALLOCATE_NODE(parser);
        root->nodeType = NodeType::IDENTIFIER;
        root->data = token.data;
        Symbol* sym = GET_SYMBOL(parser, *root->data);
        if(!sym)
        {
            triggerParserError(parser, 1, "%s is unrecognized symbol", root->data->c_str());
        }
        return root;
    }
    case TokenType::CONSTANT:
        root = ALLOCATE_NODE(parser);
        root->nodeType = NodeType::CONSTANT;
        root->data = token.data;
        return root;
    case TokenType::L_PARENTHESES:
        root = parseExpression(parser);
        parser->scanner->consume(TokenType::R_PARENTHESES);
        return root;
    default:
        triggerParserError(parser, 0, 
        "Token %s is not allowed at this position\n", tokenTypeString[(int)token.type]);
        break;
    }

    return nullptr;
}

