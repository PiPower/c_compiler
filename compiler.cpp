#include "compiler.hpp"
#include "frontend/scanner.hpp"
#include "frontend/parser.hpp"
#include "frontend/node_allocator.hpp"
#include "backend/code_gen.cpp"
using namespace std;

void compile(const char *file)
{
    SymbolTable symtab;
    initSymbolTalbe(&symtab);
    NodeAllocator allocator;
    Scanner scanner(file);

    vector<AstNode*> statements = parse(&scanner, &symtab, &allocator);

    CodeGenerator gen;
    gen.symtab = &symtab;
    gen.parseTrees = &statements;
    generate_code(&gen);

    int x = 2;
}

void initSymbolTalbe(SymbolTable *symTab)
{
    // config for x86-64
    symTab->parent = nullptr;
    symTab->scopeLevel = 0;
    // 1 byte types
    symTab->symbols["char"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 1};
    symTab->symbols["signed char"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 1};
    symTab->symbols["unsigned char"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 1};
    // 2 byte types
    symTab->symbols["short"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symTab->symbols["short int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symTab->symbols["signed short"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symTab->symbols["signed short int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symTab->symbols["unsigned short"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symTab->symbols["unsigned short int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    // 4 byte types
    symTab->symbols["int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symTab->symbols["signed"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symTab->symbols["signed int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symTab->symbols["unsinged"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symTab->symbols["unsinged int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    // 8 byte types 
    symTab->symbols["long"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["long int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["signed long"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["signed long int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["unsigned long"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["unsigned long int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["long long"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["long long int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["signed long long"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["signed long long int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["unsigned long long"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    symTab->symbols["unsigned long long int"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
    // floats
    symTab->symbols["float"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symTab->symbols["double"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};
}
