#include "compiler.hpp"
#include "frontend/scanner.hpp"
#include "frontend/parser.hpp"
#include "frontend/node_allocator.hpp"
using namespace std;


void compile(const char *file)
{
    SymbolTable symtab;
    initSymbolTalbe(&symtab);
    NodeAllocator allocator;
    Scanner scanner(file);

    vector<AstNode*> statements = parse(&scanner, &symtab, &allocator);
}

void initSymbolTalbe(SymbolTable *symTab)
{
    symTab->parent = nullptr;
    symTab->scopeLevel = 0;
    symTab->symbols["int8"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 1};
    symTab->symbols["int16"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symTab->symbols["int32"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symTab->symbols["int64"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
}
