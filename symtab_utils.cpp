#include "symtab_utils.hpp"

Symbol* getSymbol(SymbolTable *symtab, const std::string &name, uint64_t* scopeLevel)
{
    SymtabIter iter = symtab->symbols.find(name);
    if(iter != symtab->symbols.cend())
    {
        return (Symbol*)(*iter).second;
    }
    return nullptr;
}