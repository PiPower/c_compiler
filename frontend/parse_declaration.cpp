#include "parser_internal.hpp"
using namespace std;

AstNode *parseDeclaration(ParserState *parser)
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

    string* typeName = parseDeclSpec(parser);
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
            AstNode* initDeclarator = ALLOCATE_NODE(parser);
            initDeclarator->nodeType = NodeType::INIT_DECLARATOR;
            initDeclarator->children.push_back(declarator);

            if( PEEK_TOKEN(parser).type == TokenType::EQUAL )
            {
                CONSUME_TOKEN(parser, TokenType::EQUAL);
                AstNode* initializer = parseInitializer(parser);
                initDeclarator->children.push_back(initializer);
            }
            declarator = initDeclarator;
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
    Token token = GET_TOKEN(parser);
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
    if(PEEK_TOKEN(parser).type != TokenType::L_PARENTHESES)
    {
        return root;
    }

    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    if(PEEK_TOKEN(parser).type == TokenType::R_PARENTHESES)
    {
        CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
        root->nodeType = NodeType::FUNCTION_DECL;
        return root;
    }

    return root;
}
