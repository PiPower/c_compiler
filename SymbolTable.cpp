#include "SymbolTable.hpp"
#include <vector>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#define SYMBOL_NAME_PAGE_SIZE 30 * 4096

SymbolTable::SymbolTable()
{
    tableBufferHandle.push_back({});
    globalTable = &tableBufferHandle.back();
    currentTable = &tableBufferHandle.back();

    symNameBuff.offsetIntoPage = SYMBOL_NAME_PAGE_SIZE;
}

std::string_view SymbolTable::AddSymbolName(const char *symName)
{
    std::vector<char*>& filenamePages = symNameBuff.pages;
    size_t& currentPage = symNameBuff.currentPage;
    int64_t& offsetIntoPage =  symNameBuff.offsetIntoPage;
    size_t filenameLen = strlen(symName);
    
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

void SymbolTable::AddSymbolImpl(std::string_view name, Symbol *sym)
{
    if(currentTable->table.find(name) != currentTable->table.end())
    {
        printf("Symbol already exists inside symbol table \n");
        exit(-1);
    }

    currentTable->table[name] = sym;
}

Sym::Kind SymbolTable::QuerySymKind(const std::string_view& name)
{
    ScopedSymbolTable* scopedTable = currentTable;
    do 
    {
        auto symIter = scopedTable->table.find(name);
        if(symIter != scopedTable->table.end())
        {
            return symIter->second->kind;
        }
    }while ( (scopedTable = scopedTable->parent) );

    return Sym::NONE;
}
