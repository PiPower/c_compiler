#include "parser.hpp"
#include "parser_internal.hpp"
#define ERR_BUFF_SIZE 10000

using namespace std;
std::vector<AstNode *> parse(Scanner *scanner, SymbolTable *symtab, NodeAllocator *allocator)
{
    ParserState parser;
    parser.allocator = allocator;
    parser.scanner = scanner;
    parser.symtab = symtab;
    parser.errorMessage = new char[ERR_BUFF_SIZE];
    parser.errorMessageLen = ERR_BUFF_SIZE;
    parser.errCode = 0;

    setjmp(parser.jmpBuff);
    if( parser.errorMessage )
    {
        freeAllNodes(parser.allocator);
    }
    
    vector<AstNode *> statements; 
    while (parser.scanner->peekToken().type != TokenType::END_OF_FILE)
    {
        statements.push_back( parseStatement(&parser) );
    }
    return statements;
}


