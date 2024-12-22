#include "parser_internal.hpp"
using namespace std;

AstNode *parseDeclaration(ParserState *parser)
{
    Token token = PEEK_TOKEN(parser);
    if(token.type != TokenType::IDENTIFIER && token.type != TokenType::TYPE )
    {
        return nullptr;
    }
    auto symbolIter = parser->symtab->symbols.find((*(string *)token.data));
    if(symbolIter == parser->symtab->symbols.cend())
    {
        return nullptr;
    }
    
    Symbol* sym = (*symbolIter).second;
    if(sym->type != SymbolClass::TYPE)
    {
        return nullptr;
    }

    return nullptr;
}