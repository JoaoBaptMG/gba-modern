//--------------------------------------------------------------------------------
// Tileset.hpp
//--------------------------------------------------------------------------------
// Provides the TilesetData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

constexpr u32 NumTiles = 256;
enum Flags { Is8bpp = 1 };
enum TileFlags { Solid = 1 };

struct alignas(u32) Tile final
{
    u16 topLeft, topRight, bottomLeft, bottomRight;
};

struct alignas(u32) TilesetData final
{
    Tile tiles[NumTiles];
    u8 tileFlags[NumTiles];

    u16 charDataSize;
    u8 numPalettes, flags;

    const void *chars;
    const void *palette;
};
