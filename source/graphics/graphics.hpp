//--------------------------------------------------------------------------------
// graphics.hpp
//--------------------------------------------------------------------------------
// Provides functions to isolate the graphics operations
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <array>

#include "OamManager.hpp"
#include "HblankDma.hpp"
#include "SpriteSize.hpp"

namespace graphics
{
    // No tile marker
    constexpr u16 NoTile = 0xFFFF;

    // Initialize the graphics module
    void init();

    // Should be run at vblank
    void vblank();

    // Updates the graphics module
    void update();

    // Schedule a DMA copy
    void* newCopyCommand32(void* dst, u16 count);

    // Schedule a vertical copy (copy two tiles every line)
    void* newVerticalCopyCommand32(void* dst, u16 count);

    // Schedule a DMA copy from the ROM
    void romCopyCommand32(void* dst, const void* src, u16 count);

    // Resets the object allocation stack
    void resetObjectsAndPalettes();

    // Allocates a new object with the buddy allocator
    u16 allocateObjTiles(int logBlocks);

    // Frees the object
    void freeObjTiles(u16 blocks);

    // Grabs new palette entries
    void allocateObjPalettes(u32 numPalettes, u16* indices);

    // Return the palette space allocated for the palette
    void freeObjPalettes(u32 numPalettes, const u16* indices);

    // Commit the palettes needed to fill
    void commitFreePalettes();

    // The Oam manager
    extern OamManager oam;

    // The HDMA manager
    extern HblankDma hdma;
}
