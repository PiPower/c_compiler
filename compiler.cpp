#include "compiler.hpp"
#include "frontend/scanner.hpp"
#include "frontend/parser.hpp"
#include "frontend/node_allocator.hpp"
using namespace std;


void compile(const char *file)
{
    SymbolTable symtab;
    NodeAllocator allocator;
    Scanner scanner(file);

    vector<AstNode*> statements = parse(&scanner, &symtab, &allocator);

}