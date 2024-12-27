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
    // possible pointer support in future
    return parseDirectDeclarator(parser);
}

AstNode *parseInitializer(ParserState *parser)
{
    return assignmentExpression(parser);
}

std::string* parseDeclSpec(ParserState *parser)
{
    Token token = PEEK_TOKEN(parser);
    if(token.type != TokenType::IDENTIFIER && token.type != TokenType::TYPE)
    {
        return nullptr;
    }

    Symbol* symType = GET_SYMBOL(parser, *token.data);
    if(!symType)
    {
        // node plays role of a handle to unused pointer 
        AstNode* tempNode = ALLOCATE_NODE(parser);
        tempNode->data = token.data;
        triggerParserError(parser, 1, "Identifier \"%s\" has not been recognized\n", ((string*)token.data)->c_str());
    }

    if(symType->type != SymbolClass::TYPE)
    {
        return nullptr;
    }
    CONSUME_TOKEN(parser, token.type);
    return token.data;
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
     ( sym->type != SymbolClass::VARIABLE  &&  sym->type != SymbolClass::FUNCTION) )
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
        if(declarator->nodeType != NodeType::IDENTIFIER)
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

AstNode *parseArgExprList(ParserState *parser)
{
    AstNode* args = ALLOCATE_NODE(parser);
    args->nodeType = NodeType::ARGS_EXPR_LIST;
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
        symVar->type = SymbolClass::VARIABLE;
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
