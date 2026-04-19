#pragma once
#include <stdint.h>
#include <vector>
#include <cstddef>
constexpr uint64_t CPU_PAGE_SIZE = 4096;

struct PagedHeap
{
    PagedHeap(uint32_t nrOfPages);
    PagedHeap(PagedHeap& ) = delete;
    PagedHeap& operator=(PagedHeap&) = delete;
    uint64_t GetRemainingMemory();
    uint64_t GetAllocSize();
    char* allocateAligned(uint64_t size, uint64_t alignment);

    template <typename Type>
    inline Type* allocate()
    {
        char* data = allocateAligned(sizeof(Type), alignof(std::max_align_t));
        return new (data)Type;
    }
    template <typename Type>
    inline Type* allocateArray(uint64_t count)
    {
        char* data = allocateAligned(count * sizeof(Type), alignof(std::max_align_t));
        return new (data)Type[count];
    }
    ~PagedHeap();

    std::vector<void*> basePtrs;
    size_t currentPage;
    uint64_t pageOffset;
    uint32_t nrOfPages;
};
