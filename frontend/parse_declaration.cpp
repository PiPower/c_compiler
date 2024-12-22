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
        return nullptr;
    }
    AstNode* declaration = parseInitDeclList(parser);
    CONSUME_TOKEN(parser, TokenType::SEMICOLON);

    return nullptr;
}

AstNode *parseInitDeclList(ParserState *parser)
{
    do
    {
        // parse init declarator
        AstNode* declarator = parseDeclarator(parser);
        if( PEEK_TOKEN(parser).type == TokenType::EQUAL )
        {
            CONSUME_TOKEN(parser, TokenType::EQUAL);
            AstNode* initializer = parseInitializer(parser);
        }

    } while (PEEK_TOKEN(parser).type == TokenType::COMMA);

    
    return nullptr;
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
    return nullptr;
}
