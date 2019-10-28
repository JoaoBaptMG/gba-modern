//--------------------------------------------------------------------------------
// graphics.hpp
//--------------------------------------------------------------------------------
// Provides functions to isolate the graphics operations
//--------------------------------------------------------------------------------

#pragma once

#include <tonc.h>
#include <array>

#include "graphics/SpriteSize.hpp"

namespace graphics
{
    // No tile marker
    constexpr u16 NoTile = 0xFFFF;

    // Initialize the graphics module
    void init();

    // Updates the graphics module
    void update();

    // Push a sprite to the shadow OAM
    void pushOAM(u16 attr0, u16 attr1, u16 attr2);

    // Schedule a DMA copy
    void* newCopyCommand32(void* dst, u16 count);

    // Schedule a vertical copy (copy two tiles every line)
    void* newVerticalCopyCommand32(void* dst, u16 count);

    // Schedule a DMA copy from the ROM
    void romCopyCommand32(void* dst, const void* src, u16 count);

    // Resets the object allocation stack
    void resetObjectsAndPalettes();

    // Allocates a new object with the buddy allocator
    u16 allocateObjTiles(SpriteSize size);

    // Frees the object
    void freeObjTiles(u16 blocks);

    // Grabs new palette entries
    void allocateObjPalettes(u32 numPalettes, u16* indices);

    // Return the palette space allocated for the palette
    void freeObjPalettes(u32 numPalettes, const u16* indices);
}
