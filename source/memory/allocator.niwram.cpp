//--------------------------------------------------------------------------------
// allocator.niwram.cpp
//--------------------------------------------------------------------------------
// Provides a very rudimentary allocator for memory in EWRAM
//--------------------------------------------------------------------------------
#include "allocator.hpp"

#include <tonc.h>
#include <algorithm>
#include "util/gba-assert.hpp"

constexpr auto MaxFreeSlots = 32;

struct FreeSlot
{
    char* ptr;
    std::size_t size;
};

// Initialize the first free slot
extern "C" char __eheap_start[], __eheap_end[];

in_ewram_tag in_ewram;
static FreeSlot freeSlots[MaxFreeSlots];
static std::size_t numFreeSlots;

void ewram::init()
{
    numFreeSlots = 1;
    freeSlots[0].ptr = __eheap_start;
    freeSlots[0].size = __eheap_end - __eheap_start;
}

void* ewram::allocate(std::size_t sz)
{
    // Align size to the nearest word
    std::size_t im = 0;

    // Pick up the biggest size one, in order to avoid fragmentations 
    for (std::size_t i = 1; i < numFreeSlots; i++)
        if (freeSlots[i].size > freeSlots[im].size) im = i;

    // Assert that we have enough space
    ASSERT(freeSlots[im].size >= sz);

    void* result = freeSlots[im].ptr;
    freeSlots[im].ptr += sz;
    freeSlots[im].size -= sz;

    // If the slot is now empty, remove it from the list
    if (freeSlots[im].size == 0)
    {
        for (std::size_t i = numFreeSlots-1; i > im; i--)
        {
            freeSlots[i-1].ptr = freeSlots[i].ptr;
            freeSlots[i-1].size = freeSlots[i].size;
        }

        numFreeSlots--;
    }

    return result;
}

void ewram::free(void* ptr, std::size_t sz)
{
    // First, search if a free slot can be allocated
    auto slot = std::find_if(freeSlots, freeSlots+numFreeSlots,
        [=](const FreeSlot& fs) { return (char*)ptr + sz == fs.ptr; });

    // If so, just merge it
    if (slot != freeSlots+numFreeSlots)
    {
        slot->ptr = (char*)ptr;
        slot->size += sz;
    }
    else
    {
        ASSERT(numFreeSlots < MaxFreeSlots);
        numFreeSlots++;

        slot->ptr = (char*)ptr;
        slot->size = sz;
    }
}
