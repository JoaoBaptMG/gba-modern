//--------------------------------------------------------------------------------
// BuddyObjectAllocator.hpp
//--------------------------------------------------------------------------------
// Provide a tile memory allocator for the VRAM, for the object tiles
//--------------------------------------------------------------------------------

#pragma once

#include <tonc.h>
#include "graphics/SpriteSize.hpp"

class BuddyObjectAllocator final
{
    // Log 2 of the number of tiles that can be allocated
    enum : u32 { LogNumTiles = 10, NumTiles = 1 << LogNumTiles };

    // The block metadata: the linked list and the free list
    struct Block { u16 prev, next; };
    Block blocks[NumTiles];
    u16 freeList[LogNumTiles+1];

public:
    // Constructors and destructors
    BuddyObjectAllocator();
    ~BuddyObjectAllocator() {}

    // Allocators and deleters
    u16 allocBlock(SpriteSize size);
    void freeBlock(u16 tile);
};