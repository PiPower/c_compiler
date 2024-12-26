#include "parser_internal.hpp"
using namespace std;

AstNode *parseDeclaration(ParserState *parser)
{
    string* typeName = parseDeclSpec(parser);
    if(!typeName)
    {
        return nullptr;
    }
    //parse decl and possible free unused nodes
    if(PEEK_TOKEN(parser).type == TokenType::SEMICOLON)
    {
        delete typeName;
        CONSUME_TOKEN(parser, TokenType::SEMICOLON);
        return PARSER_SUCC;
    }
    AstNode* declaration = parseInitDeclList(parser, typeName);
    CONSUME_TOKEN(parser, TokenType::SEMICOLON);
    delete typeName;

    return declaration;
}

AstNode *parseFunction(ParserState *parser, AstNode *function)
{
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    if(PEEK_TOKEN(parser).type == TokenType::R_PARENTHESES)
    {
        CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
        function->nodeType = NodeType::FUNCTION_DECL;
        return function;
    }
    AstNode* args = parseFnArgs(parser);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    if(PEEK_TOKEN(parser).type != TokenType::R_PARENTHESES )
    {
        function->children.push_back(args);
        function->nodeType = NodeType::FUNCTION_DECL;
        return function;
    }
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
        if(declarator->nodeType == NodeType::FUNCTION_DECL )
        {
            SymbolFunction* func = addFunctionToSymtab(parser, *(string*)declarator->data);
            func->retType = new string(*typeName); 
        }
        else
        {
            SymbolVariable* var = addVariableToSymtab(parser, *(string*)declarator->data);
            var->varType = new string(*typeName); 
            declarator = parseInitDeclarator(parser, declarator);
        }
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

    auto symbolIter = parser->symtab->symbols.find((*(string *)token.data));
    if(symbolIter == parser->symtab->symbols.cend())
    {
        // node plays role of a handle to unused pointer 
        AstNode* tempNode = ALLOCATE_NODE(parser);
        tempNode->data = token.data;
        triggerParserError(parser, 1, "Identifier \"%s\" has not been recognized\n", ((string*)token.data)->c_str());
    }

    Symbol* sym = (*symbolIter).second;
    if(sym->type != SymbolClass::TYPE)
    {
        return nullptr;
    }
    CONSUME_TOKEN(parser, token.type);
    return (string* )token.data;
}

AstNode *parseDirectDeclarator(ParserState *parser)
{ 
    if(PEEK_TOKEN(parser).type != TokenType::IDENTIFIER)
    {
        triggerParserError(parser, 1, "Expected token is identifier but given is %s");
    }
    Token token = GET_TOKEN(parser);
    AstNode* root = ALLOCATE_NODE(parser);
    root->data = token.data;
    root->nodeType = NodeType::IDENTIFIER;
    // return if variable else function
    if(PEEK_TOKEN(parser).type != TokenType::L_PARENTHESES)
    {
        return root;
    }

    return parseFunction(parser, root);
}

AstNode *parseInitDeclarator(ParserState *parser, AstNode *declarator)
{
    AstNode* initDeclarator = ALLOCATE_NODE(parser);
    initDeclarator->nodeType = NodeType::INIT_DECLARATOR;
    initDeclarator->children.push_back(declarator);

    if( PEEK_TOKEN(parser).type == TokenType::EQUAL )
    {
        CONSUME_TOKEN(parser, TokenType::EQUAL);
        AstNode* initializer = parseInitializer(parser);
        initDeclarator->children.push_back(initializer);
    }
    return initDeclarator;
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
    AstNode* root = parseParameterTypeList(parser);
    if(!root)
    {
        return parseArgExprList(parser);
    }
    return root;
}

