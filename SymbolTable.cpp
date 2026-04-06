#include "SymbolTable.hpp"
#include <vector>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#define  SYMBOL_HEAP_DATA_PAGE_SIZE 60 * 4096
#define SYMBOL_NAME_PAGE_SIZE 30 * 4096

constexpr uint8_t fn = 0;
constexpr uint8_t type = 1;
constexpr uint8_t typeDef = 2;
constexpr uint8_t var = 3;
constexpr uint16_t allSymbols = Sym::TYPE |  Sym::TYPEDEF;

SymbolTable::SymbolTable()
{
    tableBufferHandle.push_back({});
    globalTable = &tableBufferHandle.back();
    currentTable = &tableBufferHandle.back();
    currentTable->scopeType = Scope::GLOBAL;

    symNameBuff.offsetIntoPage = SYMBOL_NAME_PAGE_SIZE;
    symbolHeap.offsetIntoPage = SYMBOL_HEAP_DATA_PAGE_SIZE;
}

std::string_view SymbolTable::AddSymbolName(const char *symName)
{
    std::vector<char*>& filenamePages = symNameBuff.pages;
    size_t& currentPage = symNameBuff.currentPage;
    int64_t& offsetIntoPage =  symNameBuff.offsetIntoPage;
    size_t filenameLen = strlen(symName);
    if(filenameLen == 0) 
    {
        return "";
    }

    if(filenameLen > SYMBOL_NAME_PAGE_SIZE )
    {
        printf("Symbol name too long\n"); exit(-1);
    }

    if(filenameLen + offsetIntoPage > SYMBOL_NAME_PAGE_SIZE)
    {
        void* mmapRet = mmap(nullptr, SYMBOL_NAME_PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(mmapRet == MAP_FAILED )
        {
            printf("mmap failed \n");
            exit(-1);
        }
        
        char* filePage = (char*)mmapRet;
        filenamePages.push_back(filePage);
        
        offsetIntoPage = 0;
        currentPage = filenamePages.size() - 1;
    }

    char* page = filenamePages[currentPage];
    memcpy(page + offsetIntoPage, symName, filenameLen);
    std::string_view view(page + offsetIntoPage,  filenameLen);
    offsetIntoPage += filenameLen;

    return view;
}

void SymbolTable::AddSymbolImpl(const std::string_view& name, Symbol *sym)
{
    bool triggerError = false;
    uint8_t scopeType;
    uint8_t prevScope;
    // only correct sequence is GLOBAL, (LOCAL, ..., LOCAL)_opt, (TYPE, ..., TYPE)_opt
    if((sym->kind == Sym::TYPE && QueryTypeSymbol(name, &scopeType, &prevScope)) )
    {
        if(prevScope == Scope::NONE || /*symbol exists in the same scope*/
           scopeType == Scope::STRUCT ) /*symbol repetition is not allowed inside nested structs*/
        {
            triggerError = true;
        }
    }

    if(triggerError)
    {
        char* c = (char* )alloca(name.length() + 1);
        memcpy(c, name.data(), name.length());
        c[name.length()] = '\0';
        printf("Symbol '%s' redefinition \n", c);
        exit(-1);
    }

    switch (sym->kind)
    {
    case Sym::TYPE:
        currentTable->tables[type][name] = sym;
        break;
    case Sym::TYPEDEF:
        currentTable->tables[typeDef][name] = sym;
        break;  
    default:
        printf("Incorrect table\n");
        exit(-1);
        break;
    }
}

Symbol* SymbolTable::QuerySymbolGeneric(
    const std::string_view &name,
    uint8_t tableIdx,
    uint8_t *scopeType,
    uint8_t* prevScope)
{
    ScopedSymbolTable* scopedTable = currentTable;
    uint8_t prevScopeV = Scope::NONE;
    do 
    {
        auto symIter = scopedTable->tables[tableIdx].find(name);
        if(symIter != scopedTable->tables[tableIdx].end())
        {
            if(scopeType) {*scopeType = scopedTable->scopeType;}
            if(prevScope) {*prevScope = prevScopeV;}
            return symIter->second;
        }
        prevScopeV = scopedTable->scopeType;
    }while ((scopedTable = scopedTable->parent));

    return nullptr;
}

SymbolType* SymbolTable::QueryTypeSymbol(
    const std::string_view& name,
    uint8_t* scopeType,
    uint8_t* prevScope)
{
    return (SymbolType*)QuerySymbolGeneric(name, type, scopeType, prevScope);
}

SymbolTypedef* SymbolTable::QueryTypedefSymbol(
    const std::string_view &name,
    uint8_t *scopeType,
    uint8_t *prevScope)
{
    return (SymbolTypedef*)QuerySymbolGeneric(name, typeDef, scopeType, prevScope);

}

uint16_t SymbolTable::QuerySymKinds(const std::string_view& name)
{
    uint16_t detectedSyms = Sym::NONE;
    ScopedSymbolTable* scopedTable = currentTable;
    do 
    {
        for(size_t i = 0; i < scopedTable->tables.size(); i++)
        {
            auto symIter = scopedTable->tables[i].find(name);
            if(symIter != scopedTable->tables[i].end())
            {
                return detectedSyms |= symIter->second->kind;
            }
        }
    }while ( (scopedTable = scopedTable->parent) && detectedSyms != allSymbols );

    return detectedSyms;
}

void SymbolTable::CreateNewScope(Scope::Type scopeType)
{
    tableBufferHandle.push_back({});
    tableBufferHandle.back().parent = currentTable;
    currentTable = &tableBufferHandle.back();
    currentTable->scopeType = scopeType;
}

void SymbolTable::PopScope()
{
    if(!currentTable->parent)
    {
        return;
    }
    currentTable = currentTable->parent;
}

char *SymbolTable::HeapAllocateAligned(uint64_t size, uint8_t alignment)
{
    if(size == 0) 
    {
        return nullptr;
    }

    std::vector<char*>& filenamePages = symbolHeap.pages;
    size_t& currentPage = symbolHeap.currentPage;
    int64_t& offsetIntoPage =  symbolHeap.offsetIntoPage;
    
    if(size > SYMBOL_HEAP_DATA_PAGE_SIZE )
    {
        printf("Symbol name too long\n"); exit(-1);
    }

    int64_t alignmentOffset = offsetIntoPage % alignment;

    if(size + offsetIntoPage + alignmentOffset >= SYMBOL_HEAP_DATA_PAGE_SIZE)
    {
        void* mmapRet = mmap(nullptr, SYMBOL_HEAP_DATA_PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(mmapRet == MAP_FAILED )
        {
            printf("mmap failed \n");
            exit(-1);
        }
        
        char* filePage = (char*)mmapRet;
        filenamePages.push_back(filePage);
        offsetIntoPage = 0;
        currentPage = filenamePages.size() - 1;
    }
    else
    {
        offsetIntoPage += alignmentOffset;
    }

    char* page = filenamePages[currentPage];
    char* data = page + offsetIntoPage;
    memset(data, 0, size);
    offsetIntoPage += size;

    return data;
}
