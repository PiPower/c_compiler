#include "SymbolTable.hpp"
#include <vector>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#define SYMBOL_NAME_PAGE_SIZE 30 * 4096

SymbolTable::SymbolTable()
{
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
    offsetIntoPage += filenameLen;

    return std::string_view(page + offsetIntoPage,  filenameLen);
}

void SymbolTable::AddSymbol(const char *name, Symbol *sym)
{
}
