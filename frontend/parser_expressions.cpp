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
    TokenType::BANG, TokenType::TILDE, TokenType::PLUS, TokenType::STAR
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

AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction, 
                    AstNode* root, 
                    const vector<TokenType>& types)
{
   return parseLoop(parser, parsingFunction, root, types.data(), types.size());
}

AstNode *parseLoop(ParserState *parser,
                    parseFunctionPtr parsingFunction, 
                    AstNode *root, 
                    const TokenType *types, 
                    const uint64_t typesCount)
{
    while (parser->scanner->currentTokenOneOf(types, typesCount))
    {
        Token operatorToken = parser->scanner->getToken();
        AstNode* parent = ALLOCATE_NODE(parser);
        parent->nodeType = tokenMathTypeToNodeType(operatorToken);
        AstNode* right = parsingFunction(parser);

        parent->children.push_back(root);
        parent->children.push_back(right);
        parent->type = typeConversion(parser, root, right);
        root = parent;
    }

    return root;
}

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
        assignement->children.push_back(nodes.top());
        assignement->children.push_back(root);
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
        root->type = new string(*root->children[0]->type);
        return root;
    }
    AstNode* root = ALLOCATE_NODE(parser);
    root->children.push_back(castExpression(parser));
    switch (token.type)
    {
        case TokenType::BANG:
            root->nodeType = NodeType::LOG_NOT; 
            root->type = new string ("char");
            break;
        case TokenType::TILDE: 
            root->nodeType = NodeType::COMPLIMENT; 
            root->type = new string(*root->children[0]->type);
            break;
        case TokenType::PLUS: root->nodeType = NodeType::ADD; break;
        case TokenType::STAR:
            root->nodeType = NodeType::DREF_PTR; 
            root->type = drefPtrType(root->children[0]->type);
            break;
    }
    return root;
}

AstNode *postfixExpression(ParserState *parser)
{
    static const TokenType postfixOperators[] = {TokenType::L_PARENTHESES, TokenType::DOT};
    static constexpr uint64_t len = sizeof(unaryOperators)/sizeof(TokenType);

    AstNode* postfixExpr = primaryExpression(parser);
    while (parser->scanner->currentTokenOneOf(postfixOperators, len))
    {
        switch (PEEK_TOKEN(parser).type)
        {
        case TokenType::L_PARENTHESES: postfixExpr = parseFnCall(parser, postfixExpr); break;
        case TokenType::DOT: postfixExpr = parseStructAccess(parser, postfixExpr); break;
        }
    }
    return postfixExpr;  
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
        if( sym->symClass == SymbolClass::VARIABLE)
        {
            root->type = new string(*((SymbolVariable*) sym)->varType);
        }
        return root;
    }
    case TokenType::CONSTANT:
        root = ALLOCATE_NODE(parser);
        root->nodeType = NodeType::CONSTANT;
        root->data = token.data;
        processConstant(parser, root);
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

AstNode *parseFnCall(ParserState *parser, AstNode* root)
{
    AstNode* fnCall = ALLOCATE_NODE(parser);
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    AstNode* args = parseArgExprList(parser);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    fnCall->nodeType = NodeType::FUNCTION_CALL;
    fnCall->children = {root, args};
    validateFnCall(parser, fnCall);

    return fnCall;  
}

void validateFnCall(ParserState *parser, AstNode *root)
{
    string* symbol = new string((*root->children[0]->data));
    Symbol* sym = GET_SYMBOL(parser, *symbol);
    if(!sym)
    {
        AstNode* handler = ALLOCATE_NODE(parser);
        handler->data = symbol;
        triggerParserError(parser, 1 , "there is no declared function named %s", symbol->c_str());
    }
    if(sym->symClass != SymbolClass::FUNCTION)
    {
        AstNode* handler = ALLOCATE_NODE(parser);
        handler->data = symbol;
        triggerParserError(parser, 1 , "symbol %s does not name a function", symbol->c_str());
    }
    SymbolFunction* symFn = (SymbolFunction*)sym;
    root->type =  new string(*symFn->retType);
}

AstNode *parseArgExprList(ParserState *parser)
{
    AstNode* args = ALLOCATE_NODE(parser);
    args->nodeType = NodeType::ARGS_EXPR_LIST;
    if(PEEK_TOKEN(parser).type == TokenType::R_PARENTHESES)
    {
        return args;
    }
    bool executeLoop = true;
    do
    {
        AstNode* expr = assignmentExpression(parser);
        args->children.push_back(expr);
        executeLoop = PEEK_TOKEN(parser).type == TokenType::COMMA;
        if(executeLoop){ POP_TOKEN(parser);}
    }while (executeLoop);
    return args;
}

AstNode *parseStructAccess(ParserState *parser, AstNode *root)
{
    CONSUME_TOKEN(parser, TokenType::DOT);
    Token token = GET_TOKEN(parser);
    if(token.type != TokenType::IDENTIFIER)
    {
        triggerParserError(parser, 1, "expected identifier\n");
    }
    AstNode* access = ALLOCATE_NODE(parser);
    access->nodeType = NodeType::ACCESS;
    access->data = token.data;
    access->children.push_back(root);
    validateStructAccess(parser, access);
    return access;
}

void validateStructAccess(ParserState *parser, AstNode *root)
{
    string::size_type pos = (*root->children[0]->type).find('|');
    string* symbol = new string((*root->children[0]->type).substr(0, pos));
    Symbol* sym = GET_SYMBOL(parser, *symbol);
    if(!sym)
    {
        delete symbol;
        triggerParserError(parser, 1, "accessing undefined type\n");
    }
    if(sym->symClass != SymbolClass::TYPE)
    {
        delete symbol;
        triggerParserError(parser, 1, "accessing symbol that is not a type\n");
    }
    SymbolType* symType = (SymbolType*) sym;
    string* elemType = nullptr;
    for(int i =0; i < symType->names.size(); i++)
    {
        if(symType->names[i] == *root->data)
        {
            elemType = &symType->types[i];
            break;
        }
    }

    if(!elemType)
    {
        triggerParserError(parser, 1, "accessing element that is not defined inside a struct\n");
    }

    root->type = new string(*elemType);
}

std::string *drefPtrType(const std::string *ptrType)
{
    const string& type = *ptrType;
    int i;
    for(i = type.size()- 1; i >=0; i--)
    {
        if(type[i] == '#')
        {
            break;
        }
    }
    if(i == 0)
    {
        return nullptr;
    }
    string* outType = new string(type.substr(0, i) );
    return outType;
}

void processConstant(ParserState *parser, AstNode *constant)
{
    string& value = *constant->data;
    int i;
    for(i=0; i < value.size(); i++)
    {
        if(value[i] == '.')
        {
            break;
        }
    }

    if(i == value.size())
    {
        uint8_t suffix = 0;
        size_t j = value.size()- 1;
        while (value[j] == 'l' || value[j] == 'u')
        {
            suffix++;
            j--;
        }
   
        if( suffix == 3)
        {
            constant->type = new string("unsigned long long");
        }
        else if( suffix == 2)
        {
            constant->type = value[value.size()- 1] == 'l' && value[value.size()- 1] == 'l'?
                                new string("long long"):
                                new string("unsigned long");
        }
        else if( suffix == 1)
        {
            constant->type = value[value.size()- 1] == 'u'?
                                new string("unsigned"):
                                new string("long");
        }
        else
        {
            constant->type = new string("int");
        }
        value.erase(value.size()-suffix, suffix);
        *constant->type += '|';
        *constant->type += (char)0x01;
        return;
    }

    if(value[value.size()-1] == 'l')
    {
        constant->type = new string("long double");
        value.erase(value.size()-1, 1);
    }
    else if(value[value.size()-1] == 'f')
    {
        constant->type = new string("float");
        value.erase(value.size()-1, 1);
    }
    else
    {
        constant->type = new string("double");
    }
    *constant->type += '|';
    *constant->type += (char)0x01;
}

std::string* typeConversion(ParserState *parser, AstNode *left, AstNode *right)
{
    string* leftType = new string((*left->type).substr(0, (*left->type).find('|')) );
    string* rightType = new string((*right->type).substr(0, (*right->type).find('|')));
    if(*leftType == *rightType)
    {
        delete leftType;
        return rightType;
    }

    AstNode* holder = ALLOCATE_NODE(parser);
    holder->data = leftType;
    holder->type = rightType;
    triggerParserError(parser, 1, "Conversion between \"%s\" and \"%s\" is not allowed\n",
    leftType->c_str(),  rightType->c_str());
    return nullptr;
}
