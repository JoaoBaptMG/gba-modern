//--------------------------------------------------------------------------------
// MapData.hpp
//--------------------------------------------------------------------------------
// Provides the Font structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

struct GlyphData final
{
    const char* data; // 1bpp data
    std::uint8_t height;
    std::int8_t offsetX;
    std::int8_t offsetY;
    std::uint8_t advanceX:7;

    // 0 for 16-bit bitfield, 1 for 32-bit bitfield
    std::uint8_t attr:1;
};

struct Font final
{
    // The glpyhs are addressed by the glyph offset
    const GlyphData* glyphs;

    std::int16_t numGlyphs;
    std::int16_t glyphOffset;
    std::int16_t verticalStride;
    std::int16_t padding;

    const GlyphData& glyphFor(char c) const
    {
        return glyphs[c - glyphOffset];
    }
};

