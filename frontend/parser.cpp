#include "parser.hpp"

struct ParserState
{
    Scanner* scanner;
    SymbolTable* symtab;
};

std::vector<AstNode *> parse(ParserState *parserState)
{
    return std::vector<AstNode *>();
}

std::vector<AstNode *> parse(Scanner *scanner, SymbolTable *symtab)
{
    ParserState parser;
    parser.scanner = scanner;
    parser.symtab = symtab;


    
    return std::vector<AstNode *>();
}
