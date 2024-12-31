#include "parser_internal.hpp"
using namespace std;

AstNode *parseDeclaration(ParserState *parser)
{
    string* typeName = parseDeclSpec(parser);
    if(!typeName)
    {
        return nullptr;
    }
    if(PEEK_TOKEN(parser).type == TokenType::SEMICOLON)
    {
        delete typeName;
        CONSUME_TOKEN(parser, TokenType::SEMICOLON);
        return PARSER_SUCC;
    }
    AstNode* declarations = parseInitDeclList(parser, typeName);
    if(declarations->nodeType != NodeType::FUNCTION_DEF)
    {
        CONSUME_TOKEN(parser, TokenType::SEMICOLON);
    }
    delete typeName;

    return declarations;
}

AstNode *parseFunction(ParserState *parser, AstNode *function)
{
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    AstNode* args = parseFnArgs(parser);
    function->children.push_back(args);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);

    if(args->nodeType == NodeType::ARGS_EXPR_LIST)
    {
        function->nodeType = NodeType::FUNCTION_CALL;
        return function;
    }

    function->nodeType = NodeType::FUNCTION_DECL;
    if(PEEK_TOKEN(parser).type != TokenType::L_BRACE )
    {
        return function;
    }
    function->nodeType = NodeType::FUNCTION_DEF;
    return function;
}

AstNode *parseParameterTypeList(ParserState *parser)
{
    string* typeName = parseDeclSpec(parser);
    if(!typeName)
    {
        return nullptr;
    }

    AstNode* paramList = ALLOCATE_NODE(parser);
    paramList->nodeType = NodeType::PARAMETER_LIST;
    do 
    {
        AstNode* declarator;
        Token next = PEEK_TOKEN(parser);
        if(next.type != TokenType::COMMA && next.type != TokenType::R_PARENTHESES)
        {
           declarator = parseDeclarator(parser);
        }
        else
        {
            declarator = ALLOCATE_NODE(parser);
            declarator->nodeType = NodeType::TYPE_PLACEHOLDER;
        }
        
        declarator->type = new string(*typeName);
        paramList->children.push_back(declarator);
        if(PEEK_TOKEN(parser).type == TokenType::R_PARENTHESES )
        {
            return paramList;
        }
        CONSUME_TOKEN(parser, TokenType::COMMA);
        delete typeName;
        typeName = parseDeclSpec(parser);
    }while (true);

    return paramList;
}

AstNode *parseInitDeclList(ParserState *parser, const std::string* typeName)
{
    AstNode* declarationList = ALLOCATE_NODE(parser);
    declarationList->nodeType = NodeType::DECLARATION_LIST;
    bool execLoop = true;
    do
    {
        // parse init declarator
        AstNode* declarator = parseDeclarator(parser);
        declarator->type = new string(*typeName);
        if(declarator->nodeType == NodeType::FUNCTION_DEF)
        {
            if(declarationList->children.size() > 0 )
            {
                triggerParserError(parser, 1, "Function definitions alongside declarations are not allowed\n");
            }
            freeNode(parser->allocator, declarationList);
            return declarator;
        }
        declarator = parseInitDeclarator(parser, declarator);
        declarationList->children.push_back(declarator);
        execLoop = PEEK_TOKEN(parser).type == TokenType::COMMA;
        if(execLoop){ POP_TOKEN(parser);}
    } while (execLoop);

    
    return declarationList;
}

AstNode *parseDeclarator(ParserState *parser)
{
    AstNode* ptr = nullptr;
    string perPtrQualifier;
    while (CURRENT_TOKEN_ON_OF(parser, {TokenType::STAR}))
    {
        CONSUME_TOKEN(parser, TokenType::STAR);
        uint8_t qualifier = parseQualifierList(parser);
        perPtrQualifier += '*'; 
        perPtrQualifier += (char)qualifier; 
    }
    if(perPtrQualifier.size() > 0 )
    {
        ptr = ALLOCATE_NODE(parser);
        ptr = ALLOCATE_NODE(parser);
        ptr->nodeType = NodeType::POINTER;
        // in case of pointers it is vector of uints8 
        ptr->data = new string(std::move(perPtrQualifier));
    }

    AstNode* root = parseDirectDeclarator(parser);
    if(ptr)
    {
        ptr->children.push_back(root);
        root = ptr;
    }
    
    return root;
}

AstNode *parseInitializer(ParserState *parser)
{
    return assignmentExpression(parser);
}

AstNode *parseStructDeclaration(ParserState *parser)
{
    string* type = parseSpecQualList(parser);
    AstNode *structDecl = nullptr;
    if(!CURRENT_TOKEN_ON_OF(parser, {TokenType::SEMICOLON}))
    {
        structDecl = ALLOCATE_NODE(parser);
        structDecl->children.push_back(parseDeclarator(parser));
        while (TOKEN_MATCH(parser, TokenType::COMMA))
        {
            structDecl->children.push_back(parseDeclarator(parser));
        }
        
    }
    CONSUME_TOKEN(parser, TokenType::SEMICOLON);

    if(!structDecl && type)
    {
        triggerParserWarning(parser, "Empty variable declaration inside struct\n");
    }

    if(structDecl)
    {
        structDecl->type = type;
    }
    else
    {
        delete type;
    }
    return structDecl;
}

std::string *parseDeclSpec(ParserState *parser)
{
    return parseSpecQualList(parser);
}

std::string* parseSpecQualList(ParserState *parser)
{
    string* type = nullptr;
    uint8_t qualifiers = parseQualifierList(parser);
    Token token = PEEK_TOKEN(parser);

    if( (uint64_t)token.type >= (uint64_t)TokenType::UNSIGNED &&
        (uint64_t)token.type <= (uint64_t)TokenType::COMPLEX)
    {
        type = parseBuiltInType(parser);
    }
    else if(token.type == TokenType::STRUCT)
    {
        type = parseStruct(parser);
    }
    else if(token.type == TokenType::TYPEDEF)
    {
        CONSUME_TOKEN(parser, TokenType::TYPEDEF);
        type = parseStruct(parser);
        Token alias = GET_TOKEN(parser);
        if(alias.type != TokenType::IDENTIFIER)
        {
            AstNode* handle = ALLOCATE_NODE(parser);
            handle->data = alias.data;
            triggerParserError(parser, 1, "expected identifier \n");
        }
        SymbolAlias* symAlias = new SymbolAlias();
        symAlias->symClass = SymbolClass::ALIAS;
        symAlias->realName = new string(*type);
        SET_SYMBOL(parser, *alias.data, (Symbol*)symAlias);
        if(PEEK_TOKEN(parser).type != TokenType::SEMICOLON)
        {
            triggerParserError(parser, 1, "typedef ends with ; \n");
        }
    }
    else if(token.type == TokenType::IDENTIFIER)
    {
        Symbol* sym = GET_SYMBOL(parser, *token.data);
        if(!sym)
        {
            return nullptr;
        }
        if(sym->symClass == SymbolClass::ALIAS)
        {
            delete token.data;
            string name ="";
            for(int i=0; (*((SymbolAlias*)sym)->realName)[i] != '|'; i++)
            {
                name += (*((SymbolAlias*)sym)->realName)[i];
            }

            Symbol* alias = GET_SYMBOL(parser, name);
            if(alias->symClass != SymbolClass::TYPE )
            {
                triggerParserError(parser, 1, "Incorrect typedef\n");
            }
            type = new string(*((SymbolAlias*)sym)->realName);
            CONSUME_TOKEN(parser, TokenType::IDENTIFIER);
        }
    }

    if(type)
    {
        uint8_t oldQualifiers = (uint8_t) (*type)[type->length()-1];
        oldQualifiers |= qualifiers;
        (*type)[type->length()-1] = (char)oldQualifiers;
    }

    if(!type && qualifiers != EMPTY_QUALIFIERS)
    {
        triggerParserError(parser, 1, "Type qualifiers can only be used with type symbols\n");
    }
    return type;
}

std::string *parseBuiltInType(ParserState *parser)
{
    constexpr static const char* types[] = {
        "unsigned", "int", "long", "float", "double", "imaginary",
        "signed", "char", "short", "void", "bool", "complex"
    };

    Token token = PEEK_TOKEN(parser);
    string* type;
    if(!(uint64_t)token.type >= (uint64_t)TokenType::UNSIGNED &&
        (uint64_t)token.type <= (uint64_t)TokenType::COMPLEX)
    {
        return nullptr;
    }
    CONSUME_TOKEN(parser, token.type);
    type = new string(types[(uint64_t)token.type - (uint64_t)TokenType::UNSIGNED]);
    uint8_t qualifiers = parseQualifierList(parser);

    token = PEEK_TOKEN(parser);
    while ((uint64_t)token.type >= (uint64_t)TokenType::UNSIGNED &&
            (uint64_t)token.type <= (uint64_t)TokenType::COMPLEX)
    {
        *type += ' ';
        *type += types[(uint64_t)token.type - (uint64_t)TokenType::UNSIGNED];
        CONSUME_TOKEN(parser, token.type);
        qualifiers |= parseQualifierList(parser);
        token = PEEK_TOKEN(parser);
    }
    *type += '|';
    *type += (char)qualifiers;
    return type;
}

std::string *parseStruct(ParserState *parser)
{
    CONSUME_TOKEN(parser, TokenType::STRUCT);
    Token token = PEEK_TOKEN(parser);
    string* structName = nullptr;
    if(token.type == TokenType::IDENTIFIER)
    {
        POP_TOKEN(parser);
        structName = token.data;
        *structName += " struct";
        token = PEEK_TOKEN(parser);
        // struct declaration
        if(PEEK_TOKEN(parser).type == TokenType::SEMICOLON &&
            !GET_SYMBOL(parser, *structName))
        {
            SymbolType* symType = new SymbolType();
            symType->symClass = SymbolClass::TYPE;
            symType->attributes = 0x00;
            SET_SYMBOL(parser, *structName, (Symbol*)symType);
            *structName += '|';
            *structName += EMPTY_QUALIFIERS;
            return structName;
        }
        else if (PEEK_TOKEN(parser).type == TokenType::SEMICOLON)
        {
            *structName += '|';
            *structName += EMPTY_QUALIFIERS;
            return structName;
        }
    }

    if( token.type != TokenType::L_BRACE)
    {
        Symbol* sym = GET_SYMBOL(parser, *structName);
        if(sym->symClass != SymbolClass::TYPE)
        {
            AstNode* ptrHandle = ALLOCATE_NODE(parser);
            ptrHandle->data = structName;
            triggerParserError(parser, 1, "Symbol \"%s\" does not correspond to type\n", structName->c_str());
        }
        if(!isSetDefinedAttr((SymbolType*)sym))
        {
            AstNode* ptrHandle = ALLOCATE_NODE(parser);
            ptrHandle->data = structName;
            triggerParserError(parser, 1, "Symbol \"%s\" does not correspond to DEFINED type\n", structName->c_str());
        }
        *structName += '|';
        *structName += EMPTY_QUALIFIERS;
        return structName;
    }

    if(!structName)
    {
        // anonymous struct
        structName = generateAnonymousStructName(parser);
        *structName += " struct";
    }
    CONSUME_TOKEN(parser, TokenType::L_BRACE);
    parseStructDeclList(parser, structName);
    CONSUME_TOKEN(parser, TokenType::R_BRACE);
    *structName += '|';
    *structName += EMPTY_QUALIFIERS;

    return structName;
}

std::string *parseTypeName(ParserState *parser)
{
    // for now does not support pointer_opt direct-abstract-declarator
    // only pointer !!!!!
    string* type = parseSpecQualList(parser);

    while (CURRENT_TOKEN_ON_OF(parser, {TokenType::STAR}))
    {
        CONSUME_TOKEN(parser, TokenType::STAR);
        uint8_t qualifier = parseQualifierList(parser);
        *type += '*';
        *type += (char)qualifier;
    }
    
    return type;
}

void parseStructDeclList(ParserState *parser, const std::string *structName)
{
    SymbolType* symType = defineTypeSymbol(parser, structName);

    while (!CURRENT_TOKEN_ON_OF(parser, {TokenType::R_BRACE}))
    {
        AstNode* decl = parseStructDeclaration(parser);
        if(decl)
        {
            for(int i =0; i < decl->children.size(); i++)
            {
                addParameterToStruct(parser, symType, decl->children[i], decl->type);
            }
        }
        FREE_NODE_REC(parser, decl);
    }
    
}


uint8_t parseQualifierList(ParserState *parser)
{
    static vector<TokenType> types = {TokenType::CONST, TokenType::VOLATILE, TokenType::RESTRICT};
    // bit 0: always set to 1 to avoid null char
    // bit 1: const
    // bit 2: volitaile
    // bit 3: restrict
    uint8_t qualifiers = 0x01;
    while (CURRENT_TOKEN_ON_OF(parser, types))
    {
        if(TOKEN_MATCH(parser, TokenType::CONST))
        {
            qualifiers |= 0x02;
        }
        if(TOKEN_MATCH(parser, TokenType::VOLATILE))
        {
            qualifiers |= 0x04;
        }
        if(TOKEN_MATCH(parser, TokenType::RESTRICT))
        {
            qualifiers |= 0x08;
        }
    }
    
    return qualifiers;
}

AstNode *parseDirectDeclarator(ParserState *parser)
{ 
    if(PEEK_TOKEN(parser).type != TokenType::IDENTIFIER)
    {
        triggerParserError(parser, 1, "Expected token is identifier");
    }
    Token token = GET_TOKEN(parser);
    AstNode* root = ALLOCATE_NODE(parser);
    root->data = token.data;
    root->nodeType = NodeType::IDENTIFIER;

    Symbol* sym = GET_SYMBOL(parser, *root->data );
    if(sym && 
     ( sym->symClass != SymbolClass::VARIABLE  &&  sym->symClass != SymbolClass::FUNCTION) )
    {
        triggerParserError(parser, 1, "Symbol %s is not variable", root->data->c_str());
    }
    // return if variable else function
    if(PEEK_TOKEN(parser).type != TokenType::L_PARENTHESES)
    {
        return root;
    }

    return parseFunction(parser, root);
}

AstNode *parseInitDeclarator(ParserState *parser, AstNode *declarator)
{
    if( PEEK_TOKEN(parser).type == TokenType::EQUAL )
    {
        if(declarator->nodeType == NodeType::FUNCTION_DECL || 
            declarator->nodeType == NodeType::FUNCTION_DEF )
        {
            triggerParserError(parser, 1, "Assignment operator is not supported for this declarator\n");
        }

        CONSUME_TOKEN(parser, TokenType::EQUAL);

        AstNode* initializer = ALLOCATE_NODE(parser);
        initializer->nodeType = NodeType::INITALIZER;
        initializer->children.push_back(parseInitializer(parser));
        declarator->children.push_back(initializer);
    }
    return declarator;
}

AstNode *parseFnArgs(ParserState *parser)
{
    if(PEEK_TOKEN(parser).type == TokenType::R_PARENTHESES)
    {
        AstNode* emptyParamList = ALLOCATE_NODE(parser);
        emptyParamList->nodeType = NodeType::PARAMETER_LIST;
        return emptyParamList;
    }

    AstNode* root = parseParameterTypeList(parser);
    if(!root)
    {
        return parseArgExprList(parser);
    }
    return root;
}

// fills local scope symtab and order of params of function symbol
AstNode *parseFunctionBody(ParserState *parser, AstNode *function)
{
    // processDeclarationTree makes all the consistency checks, we are here if they were passed
    SymbolFunction* fnSym = (SymbolFunction*)GET_SYMBOL(parser, *function->data);
    SymbolTable* funcSymtab = new SymbolTable();
    funcSymtab->scopeLevel = parser->symtab->scopeLevel + 1;
    funcSymtab->parent = parser->symtab;
    parser->symtab = funcSymtab;

    for(int i =0; i <function->children[0]->children.size(); i++)
    {
        AstNode* param = function->children[0]->children[i];
        SymbolVariable* symVar = new SymbolVariable;
        symVar->symClass = SymbolClass::VARIABLE;
        symVar->varType = new string( *fnSym->argTypes[i]);
        setDefinedAttr(symVar);
        SET_SYMBOL(parser, *param->data, (Symbol*)symVar);

        fnSym->argNames.push_back(param->data);
        param->data = nullptr;
        freeNode(parser->allocator, param);
    }
    freeNode(parser->allocator, function->children[0]);

    AstNode* functionBlock = parseCompoundStatement(parser);
    parser->symtab = funcSymtab->parent;
    return functionBlock;
}
