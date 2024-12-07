#include "parser.hpp"
#include "node_allocator.hpp"
#include "parser_internal.hpp"
#define ERR_BUFF_SIZE 10000

std::vector<AstNode *> parse(Scanner *scanner, SymbolTable *symtab)
{
    ParserState parser;
    parser.scanner = scanner;
    parser.symtab = symtab;
    parser.errorMessage = new char[ERR_BUFF_SIZE];
    parser.errorMessageLen = ERR_BUFF_SIZE;
    parser.errCode = 0;

    setjmp(parser.jmpBuff);
    if( parser.errorMessage )
    {
        freeAllNodes(&parser.nodes);
    }
    
    while (parser.scanner->peekToken().type != TokenType::END_OF_FILE)
    {
        parser.nodes.push_back( parseStatement(&parser) );
    }
    return std::vector<AstNode *>();
}


