#include "compiler.hpp"
#include "frontend/scanner.hpp"
#include "frontend/parser.hpp"
#include "frontend/node_allocator.hpp"
#include "backend/code_gen.cpp"
using namespace std;

void compile(const char *file, FILE* stream)
{
    SymbolTable symtab;
    initSymbolTalbe(&symtab);
    NodeAllocator allocator;
    Scanner scanner(file);

    vector<AstNode*> statements = parse(&scanner, &symtab, &allocator);

    CodeGenerator gen;
    gen.symtab = &symtab;
    gen.allocator = &allocator;
    gen.cpu = nullptr; // for global scope
    gen.localSymtab = &symtab;
    generate_code(&gen, &statements);
    write_to_file(gen.code, stream);

    int x = 2;
}

void initSymbolTalbe(SymbolTable *symTab)
{
    // config for x86-64
    symTab->parent = nullptr;
    symTab->scopeLevel = 0;
    // 1 byte types
    insertSymbol(symTab, "char",  (Symbol*)(Symbol*) new SymbolType{SymbolClass::TYPE, INT8_S, 1, 1});

    insertSymbol(symTab, "char", (Symbol*) new SymbolType{SymbolClass::TYPE, INT8_S, 1, 1});
    insertSymbol(symTab, "signed char", (Symbol*) new SymbolType{SymbolClass::TYPE, INT8_S, 1, 1});
    insertSymbol(symTab, "unsigned char", (Symbol*) new SymbolType{SymbolClass::TYPE, INT8_U, 1, 1});

    // 2 byte types
    insertSymbol(symTab, "short", (Symbol*) new SymbolType{SymbolClass::TYPE, INT16_S, 2, 2});
    insertSymbol(symTab, "short int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT16_S, 2, 2});
    insertSymbol(symTab, "signed short", (Symbol*) new SymbolType{SymbolClass::TYPE, INT16_S, 2, 2});
    insertSymbol(symTab, "signed short int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT16_S, 2, 2});
    insertSymbol(symTab, "unsigned short", (Symbol*) new SymbolType{SymbolClass::TYPE, INT16_U, 2, 2});
    insertSymbol(symTab, "unsigned short int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT16_U, 2, 2});

    // 4 byte types
    insertSymbol(symTab, "int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT32_S, 4, 4});
    insertSymbol(symTab, "signed", (Symbol*) new SymbolType{SymbolClass::TYPE, INT32_S, 4, 4});
    insertSymbol(symTab, "signed int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT32_S, 4, 4});
    insertSymbol(symTab, "unsigned", (Symbol*) new SymbolType{SymbolClass::TYPE, INT32_U, 4, 4});
    insertSymbol(symTab, "unsigned int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT32_U, 4, 4});

    // 8 byte types
    insertSymbol(symTab, "long", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "long int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "signed long", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "signed long int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "unsigned long", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_U, 8, 8});
    insertSymbol(symTab, "unsigned long int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_U, 8, 8});
    insertSymbol(symTab, "long long", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "long long int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "signed long long", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "signed long long int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_S, 8, 8});
    insertSymbol(symTab, "unsigned long long", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_U, 8, 8});
    insertSymbol(symTab, "unsigned long long int", (Symbol*) new SymbolType{SymbolClass::TYPE, INT64_U, 8, 8});

    // floats
    insertSymbol(symTab, "float", (Symbol*) new SymbolType{SymbolClass::TYPE, FLOAT32, 4, 4});
    insertSymbol(symTab, "double", (Symbol*) new SymbolType{SymbolClass::TYPE, DOUBLE64, 8, 8});

    // special
    insertSymbol(symTab, "void", (Symbol*) new SymbolType{SymbolClass::TYPE, VOID_GR, 0, 0});
    insertSymbol(symTab, "*", (Symbol*) new SymbolType{SymbolClass::TYPE, POINTER_GR, 8, 8});

}

