//--------------------------------------------------------------------------------
// graphics.hpp
//--------------------------------------------------------------------------------
// Provides functions to isolate the graphics operations
//--------------------------------------------------------------------------------

#pragma once

#include <tonc.h>
#include <array>

#include "BuddyObjectAllocator.hpp"

namespace graphics
{
    // Initialize the graphics module
    void init();

    // Updates the graphics module - it is put in IWRAM
    // to improve the speed
    void update() IWRAM_CODE;

    // Push a sprite to the shadow OAM
    void pushOAM(u16 attr0, u16 attr1, u16 attr2);

    // Schedule a DMA copy
    void* newCopyCommand32(void* dst, u16 count);

    // Schedule a vertical copy (copy two tiles every line)
    void* newVerticalCopyCommand32(void* dst, u16 count);

    // Resets the object allocation stack
    void resetObjectsAndPalettes();

    // Allocates a new object with the buddy allocator
    u16 allocateObjTiles(SpriteSize size);

    // Frees the object
    void freeObjTiles(u16 blocks);

    // Grabs new palette entries
    void allocateObjPalettes(u32 numPalettes, u32* indices);

    // Return the palette space allocated for the palette
    void freeObjPalettes(u32 numPalettes, const u32* indices);
}
