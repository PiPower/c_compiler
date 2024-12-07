#include "compiler.hpp"
#include "frontend/scanner.hpp"
#include "frontend/parser.hpp"

using namespace std;


void compile(const char *file)
{
    SymbolTable symtab;
    Scanner scanner(file);

    vector<AstNode*> statements = parse(&scanner, &symtab);

}