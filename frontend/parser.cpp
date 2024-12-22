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
    parser.isParsingAssignment = false;
    parser.jmpHolder = nullptr;
    parser.errorMessage = new char[ERR_BUFF_SIZE];
    parser.errorMessageLen = ERR_BUFF_SIZE;
    parser.errCode = 0;

    if( setjmp(parser.jmpBuff) != 0)
    {
        freeAllNodes(parser.allocator);
        printf("%s", parser.errorMessage);
        return {nullptr};
    }
    
    vector<AstNode *> statements; 
    while (parser.scanner->peekToken().type != TokenType::END_OF_FILE)
    {
        AstNode* root = parseDeclaration(&parser);
        if(!root)
        {
            root = parseStatement(&parser);
        }
        statements.push_back(root);
    }
    return statements;
}


