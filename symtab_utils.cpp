#include "symtab_utils.hpp"

using namespace std;
Symbol* getSymbol(SymbolTable *symtab, const std::string &name, uint64_t* scopeLevel)
{
    if(name.find('*')!= string::npos)
    {
        if(scopeLevel)
        {
            *scopeLevel = 0;
        }
        SymbolTable * glob = symtab;
        while (glob->parent)
        {
            glob = glob->parent;
        }
        
        SymtabIter iter = glob->symbols.find("*");
        return (Symbol*)(*iter).second;
    }

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

