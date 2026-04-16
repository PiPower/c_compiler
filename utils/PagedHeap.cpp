#include "PagedHeap.hpp"
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cstdlib>

PagedHeap::PagedHeap(uint32_t nrOfPages)
:
currentPage(0), pageOffset(0), nrOfPages(nrOfPages)
{
    pageOffset = nrOfPages * CPU_PAGE_SIZE;
}

char* PagedHeap::allocateAligned(uint64_t size, uint64_t alignment)
{
    if(size == 0)
    {
        return nullptr;
    }

    if(size > nrOfPages * CPU_PAGE_SIZE)
    {
        return nullptr;
    }

	int64_t alignmentOffset = pageOffset % alignment ? alignment - pageOffset % alignment : 0;

    if(size + pageOffset + alignmentOffset > nrOfPages * CPU_PAGE_SIZE)
    {
        void* mmapRet = mmap(nullptr, nrOfPages * CPU_PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(mmapRet == MAP_FAILED )
        {
            return nullptr;
        }
        
        char* filePage = reinterpret_cast<char*>(mmapRet);
        basePtrs.push_back(filePage);
        pageOffset = 0;
        currentPage = basePtrs.size() - 1;
    }
    else
    {
        pageOffset += alignmentOffset;
    }

    void* page = basePtrs[currentPage];
    char* data = reinterpret_cast<char*>(page) + pageOffset;
    pageOffset += size;

    return data;
}

PagedHeap::~PagedHeap()
{
    for(void* basePtr : basePtrs)
    {
        if(munmap(basePtr, nrOfPages * CPU_PAGE_SIZE) != 0 )
        {
            printf("munmap error\n");
            exit(-1);
        }
    }
}
