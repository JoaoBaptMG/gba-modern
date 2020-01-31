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

    // Allocate from the end of the block, it's more performant
    freeSlots[im].size -= sz;
    void* result = freeSlots[im].ptr + freeSlots[im].size;
    
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
    // First, search if a free slot can be merged with this one
    std::size_t backMergeSlot = -1;
    std::size_t frontMergeSlot = -1;
    for (std::size_t i = 0; i < numFreeSlots; i++)
    {
        if (freeSlots[i].ptr + freeSlots[i].size == ptr) backMergeSlot = i;
        else if ((char*)ptr + sz == freeSlots[i].ptr) frontMergeSlot = i;
    }

    if (backMergeSlot != (std::size_t)-1 && frontMergeSlot != (std::size_t)-1)
    {
        // Check which slot is the first
        auto [firstSlot, secondSlot] = std::minmax(backMergeSlot, frontMergeSlot);

        // Update the pointers
        freeSlots[firstSlot].ptr = freeSlots[backMergeSlot].ptr;
        freeSlots[firstSlot].size += sz + freeSlots[secondSlot].ptr;

        // And remove the second slot
        for (std::size_t i = numFreeSlots-1; i > secondSlot; i--)
        {
            freeSlots[i-1].ptr = freeSlots[i].ptr;
            freeSlots[i-1].size = freeSlots[i].size;
        }

        numFreeSlots--;
    }
    else if (backMergeSlot != -1)
    {
        // The slot is on the back of the free pointer, just add to the size
        freeSlots[backMergeSlot].size += sz;
    }
    else if (frontMergeSlot != -1)
    {
        // The slot is on the front of the free pointer, needs to update the pointer
        freeSlots[frontMergeSlot].ptr = (char*)ptr;
        freeSlots[frontMergeSlot].size += sz;
    }
    else // No slots to merge
    {
        ASSERT(numFreeSlots < MaxFreeSlots);
        freeSlots[numFreeSlots].ptr = (char*)ptr;
        freeSlots[numFreeSlots].size = sz;
        numFreeSlots++;
    }
}
