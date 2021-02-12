//--------------------------------------------------------------------------------
// GlyphData.hpp
//--------------------------------------------------------------------------------
// Provides the GlyphData structure used by glyph writers
//--------------------------------------------------------------------------------
#pragma once


#include <cstdint>

struct GlyphData final
{
    std::uint8_t height;
    std::int8_t offsetX;
    std::int8_t offsetY;
    std::uint8_t advanceX:7;

    // 0 for 16-bit bitfield, 1 for 32-bit bitfield
    std::uint8_t attr:1;
    
    const char* data; // 1bpp data
};