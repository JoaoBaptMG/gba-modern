//--------------------------------------------------------------------------------
// BuddyObjAlloc.hpp
//--------------------------------------------------------------------------------
// Provide a tile memory allocator for the VRAM, for the object tiles
//--------------------------------------------------------------------------------

#pragma once

#include <tonc.h>

// Sprite size enum
enum class SpriteSize : u32
{
    // 4bpp sizes
    _8x8_4bpp = 0, _16x16_4bpp = 2, _32x32_4bpp = 4, _64x64_4bpp = 6,
    _16x8_4bpp = 1, _32x16_4bpp = 3, _32x8_4bpp = 2, _64x32_4bpp = 5,
    _8x16_4bpp = 1, _16x32_4bpp = 3, _8x32_4bpp = 2, _32x64_4bpp = 5,

    // 8bpp sizes
    _8x8_8bpp = 1, _16x16_8bpp = 3, _32x32_8bpp = 5, _64x64_8bpp = 7,
    _16x8_8bpp = 2, _32x16_8bpp = 4, _32x8_8bpp = 3, _64x32_8bpp = 6,
    _8x16_8bpp = 2, _16x32_8bpp = 4, _8x32_8bpp = 3, _32x64_8bpp = 6,
};

constexpr u16 NoTile = 0xFFFF;

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