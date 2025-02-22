#include "parser_internal.hpp"
#include "parser_utils.hpp"
#include <stack>
#include <algorithm>

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
    TokenType::BANG, TokenType::TILDE, TokenType::PLUS, TokenType::STAR, 
    TokenType::MINUS, TokenType::AMPERSAND
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
    TokenType::AMPERSAND
    };

static constexpr TokenType escOrTypes[] =  {
    TokenType::CARET
    };

static constexpr TokenType incOrTypes[] =  {
    TokenType::PIPE
    };

static constexpr TokenType logAndTypes[] =  {
    TokenType::DOUBLE_AMPERSAND
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
        filterUnallowedPointerArithmetic(parser, parent->nodeType, parent->type);
        root = parent;
    }

    return root;
}

AstNode *parsePtrStructAccess(ParserState *parser, AstNode *root)
{
    CONSUME_TOKEN(parser, TokenType::ARROW);
    Token token = GET_TOKEN(parser);
    if(token.type != TokenType::IDENTIFIER)
    {
        triggerParserError(parser, 1, "expected identifier\n");
    }
    AstNode* ptrAccess = ALLOCATE_NODE(parser);
    ptrAccess->nodeType = NodeType::PTR_ACCESS;
    ptrAccess->data = token.data;
    ptrAccess->children.push_back(root);
    string::difference_type n = count(
        ptrAccess->children[0]->type->begin(),  ptrAccess->children[0]->type->end(), '*');
    if(n != 1)
    {
        triggerParserError(parser, 1, "Operator \"->\" in inapriopriate for type %s \n", 
        ptrAccess->children[0]->type->c_str());
    }
    validateStructAccess(parser, ptrAccess, true);
    return ptrAccess;
}

void filterUnallowedPointerArithmetic(ParserState *parser,
                                NodeType op, 
                                const std::string* exprType)
{
    if(exprType->find('*') == string::npos)
    {
        return;
    }
 
    if(op != NodeType::ADD && op != NodeType::ADD_ASSIGNMENT 
       &&
       op != NodeType::SUBTRACT && op != NodeType::SUB_ASSIGNMENT
       &&
       op != NodeType::ASSIGNMENT)
    {
        triggerParserError(parser, 0, "Forbidden pointer operation");
    }
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
        validateAssignment(parser, nodes.top()->type, root->type);
        assignement->type = new string(*nodes.top()->type);

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
    AstNode* castOperator = parseCastingType(parser);

    if(parser->isParsingAssignment)
    {
        static constexpr uint64_t len = sizeof(assignmentTypes)/sizeof(TokenType);

        parser->isParsingAssignment = false;
        // assignmentJmp might be overwrittent by recursive
        // calls so its saved here
        jmp_buf buff ;
        buff[0] = parser->assignmentJmp[0];

        AstNode* root = unaryExpression(parser);
        if(!parser->scanner->currentTokenOneOf(assignmentTypes, len) || castOperator)
        {
            if(castOperator)
            {
                castOperator->children.push_back(root);
                root = castOperator;
            }
            return root;
        }
        parser->jmpHolder = root;
        longjmp(buff, 1);
    }

    AstNode* root = unaryExpression(parser);
    if(castOperator)
    {
        castOperator->children.push_back(root);
        root = castOperator;
    }
    return root;
}

AstNode *parseCastingType(ParserState *parser)
{
     //outer most cast is only needed to be considered
    if(PEEK_TOKEN(parser).type != TokenType::L_PARENTHESES)
    {
        return nullptr;
    }
    Token t = GET_TOKEN(parser);
    string* type = parseTypeName(parser);
    if(!type)
    {
        PUT_FRONT(parser, t);
        return nullptr;
    }
    else
    {
        CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
        // possible chain of casting, casts further down the chain
        // are useless
        AstNode* unused_type = parseCastingType(parser);
        FREE_NODE(parser, unused_type);
    }

    AstNode* cast = ALLOCATE_NODE(parser);
    cast->nodeType = NodeType::CAST;
    TypePair typeInfo = decodeType(type);
    cast->type = typeInfo.type;
    cast->data = typeInfo.qualifiers;
    delete type;
    return cast;
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
        case TokenType::PLUS: 
            root->nodeType = NodeType::ADD;
            root->type = new string(*root->children[0]->type);
            break;
        case TokenType::MINUS: 
        {
            root->nodeType = NodeType::MINUS;
            root->type = new string(*root->children[0]->type);
            uint8_t affi = getTypeAffiliation(parser, root->type);
            if(affi == STRUCT_GR || affi == VOID_GR || affi == POINTER_GR)
            {
                triggerParserError(parser, 1, "- operator has no meaning for type \"%s\"\n", root->type->c_str());
            }
            else if( affi >= INT8_U && affi <= INT64_U)
            {
                triggerParserWarning(parser, "- operator for type \"%s\" may cause loss of data\n", root->type->c_str());
            }
        }break;
        case TokenType::STAR:
            root->nodeType = NodeType::DREF_PTR; 
            root->type = drefPtrType(root->children[0]->type);
            break;
        case TokenType::AMPERSAND:
            root->nodeType = NodeType::GET_ADDR;
            processGetAddr(parser, root);
            break;
        default:
            printf("Internal Error: Unepxected token type\n");
            exit(-1);
    }
    return root;
}

AstNode *postfixExpression(ParserState *parser)
{
    static const TokenType postfixOperators[] = {TokenType::L_PARENTHESES, TokenType::DOT, TokenType::ARROW};
    static constexpr uint64_t len = sizeof(unaryOperators)/sizeof(TokenType);

    AstNode* postfixExpr = primaryExpression(parser);
    while (parser->scanner->currentTokenOneOf(postfixOperators, len))
    {
        switch (PEEK_TOKEN(parser).type)
        {
        case TokenType::L_PARENTHESES: postfixExpr = parseFnCall(parser, postfixExpr); break;
        case TokenType::DOT: postfixExpr = parseStructAccess(parser, postfixExpr); break;
        case TokenType::ARROW: postfixExpr = parsePtrStructAccess(parser, postfixExpr); break;
        default:
            printf("Internal Error: Unepxected token type\n");
            exit(-1);
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

void validateStructAccess(ParserState *parser, AstNode *root, bool ptrAccess)
{
    string* type = root->children[0]->type;
    if(ptrAccess)
    {
        type = new string(*type);
        type->pop_back();
    }
    string* symbol = new string(*type);
    if(ptrAccess)
    {
        delete type;
    }
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
    for(size_t i =0; i < symType->names.size(); i++)
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
        if(type[i] == '*')
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
    size_t i;
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
}

void validateAssignment(ParserState *parser, std::string* left, std::string* right)
{
    if(left->find('*') != string::npos &&
        right->find('*') != string::npos&&
        *left != *right)
    {
        triggerParserError(parser, 1, 
            "Pointer assignment \"%s\" <- \"%s\" is invalid\n",left->c_str(), right->c_str() );
    }

    if(*left == *right)
    {
        return;
    }

    uint8_t leftGroup = getTypeGroup(parser, left);
    uint8_t rightGroup = getTypeGroup(parser, right);
    uint16_t leftAffiliation = getTypeAffiliation(parser, left);
    uint16_t rightAffiliation = getTypeAffiliation(parser, right);
    if( rightGroup == leftGroup)
    {
        if(leftAffiliation< rightAffiliation)
        {
            triggerParserWarning(parser,  "Assignment betwen \"%s\"<-\"%s\" may cause loss of data\n", 
            left->c_str(), right->c_str());
        }
        return;
    }
    if( leftGroup != rightGroup && 
        leftGroup != VOID_GR  && leftGroup != STRUCT_GR &&
        rightGroup != VOID_GR  && rightGroup != STRUCT_GR)
    {
         triggerParserWarning(parser,  "Assignment betwen \"%s\" <- \"%s\" may cause loss of data\n", 
         left->c_str(), right->c_str());
         return;
    }

    if( leftGroup == SIGNED_INT_GROUP && rightGroup == UNSIGNED_INT_GROUP)
    {
        if(leftAffiliation >> SIGNED_INT_GROUP *4 <= rightAffiliation >> UNSIGNED_INT_GROUP *4)
        {
            triggerParserWarning(parser,  "Assignment between \"%s\" <- \"%s\" may cause loss of data\n", 
            left->c_str(), right->c_str());
        }
         return;
    }

    triggerParserError(parser, 1, "Assignment between \"%s\" <- \"%s\" is forbidden\n", 
    left->c_str(), right->c_str());
}

void processGetAddr(ParserState *parser, AstNode *getAddr)
{
    AstNode* owner = getAddr->children[0];
    if(owner->data->find('*')!= string::npos)
    {
        triggerParserError(parser, 1, "\"%s\" does not have internal elements\n", owner->data->c_str());
    }
    Symbol* sym = GET_SYMBOL(parser, *owner->data);
    if(!sym)
    {
        triggerParserError(parser, 1, "Requested symbol \"%s\" does not exist\n", owner->data->c_str());
    }
    
    if(sym->symClass != SymbolClass::VARIABLE)
    {
        triggerParserError(parser, 1, "Requested symbol \"%s\" is not lvalue\n", owner->data->c_str());
    }
    SymbolVariable* symVar = (SymbolVariable*)sym;
    getAddr->type = new string(*symVar->varType);
    *getAddr->type += '*';
    getAddr->data = new string(*symVar->qualifiers);
    *getAddr->data += EMPTY_QUALIFIERS;
}


std::string* typeConversion(ParserState *parser, AstNode *left, AstNode *right)
{
    uint8_t leftGroup;
    uint8_t rightGroup;
    uint16_t leftAffiliation;
    uint16_t rightAffiliation;

    if((*left->type == "void" || *right->type == "void" ) ||
       (left->type->find("struct") != string::npos || right->type->find("struct") != string::npos) )
    {
        goto trigger_error;
    }

    if(*left->type == *right->type)
    {
        return new string(*right->type);
    }

    leftGroup = getTypeGroup(parser, left->type);
    rightGroup = getTypeGroup(parser, right->type);
    leftAffiliation = getTypeAffiliation(parser, left->type);
    rightAffiliation = getTypeAffiliation(parser, right->type);

    if( rightGroup == leftGroup)
    {
        if((leftAffiliation == POINTER_GR && rightAffiliation == leftAffiliation)
            &&
            (*left->type != *right->type))
        {
            triggerParserError(parser, 0, 
            "implicit conversion between \"%s\" and \"%s\" is forbbiden",
            left->type->c_str(), right->type->c_str());
        }

        return copyStrongerType(parser, left->type, right->type);
    }

    if( leftGroup != rightGroup && 
        leftGroup != VOID_GR  && leftGroup != STRUCT_GR &&
        rightGroup != VOID_GR  && rightGroup != STRUCT_GR )
    {
        if(leftAffiliation == POINTER_GR)
        {
            return new string(*left->type);
        }
        else if(rightAffiliation == POINTER_GR)
        {
            return new string(*right->type);
        }

        string* targetType = resolveImpConv(parser, left->type, right->type, leftGroup, rightGroup);
        triggerParserWarning(parser, 
        "Implicit conversion between \"%s\" and \"%s\" into \"%s\", possible loss of data\n",
         left->type->c_str(), right->type->c_str(), targetType->c_str());
        return targetType;
    }

trigger_error:
    triggerParserError(parser, 1, "Implicit conversion between \"%s\" and \"%s\" is not allowed\n",
     left->type->c_str(),   right->type->c_str());
    return nullptr;
}