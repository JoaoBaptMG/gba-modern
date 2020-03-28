//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Provides the TilesetData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

struct BackgroundData final
{
    std::uint16_t tileWidth, tileHeight;
    std::uint16_t charDataSize;
    std::uint16_t numPalettes:15, is8bpp:1;
    
    const void* chars;
    const std::uint16_t* tiles;
    const std::uint16_t* palettes;
};

struct BackgroundHandle final
{
    BackgroundData png;
};
