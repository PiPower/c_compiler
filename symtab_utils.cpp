#include "symtab_utils.hpp"

Symbol* getSymbol(SymbolTable *symtab, const std::string &name, uint64_t* scopeLevel)
{
    while (symtab)
    {
        SymtabIter iter = symtab->symbols.find(name);
        if(iter != symtab->symbols.cend())
        {
            if(scopeLevel)
            {
                *scopeLevel = symtab->scopeLevel;
            }
            return (Symbol*)(*iter).second;
        }
        symtab = symtab->parent;
    }

    return nullptr;
}

Symbol *getSymbolLocal(SymbolTable *symtab, const std::string &name)
{
    SymtabIter iter = symtab->symbols.find(name);
    if(iter != symtab->symbols.cend())
    {
        return (Symbol*)(*iter).second;
    }
    return nullptr;
}
