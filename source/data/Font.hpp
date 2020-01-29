//--------------------------------------------------------------------------------
// MapData.hpp
//--------------------------------------------------------------------------------
// Provides the Font structure to be populated by the exporter tool
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

struct Font final
{
    // The glpyhs are addressed by the glyph offset
    const GlyphData* glyphs;

    std::int16_t verticalStride;
    std::int16_t padding;

    // First hword, glyph begin, second word, glyph end (exclusive)
    std::uint32_t glyphIntervals[1024];

    const GlyphData& glyphFor(int ch) const
    {
        int ofs = 0;
        for (const auto *gi = glyphIntervals; *gi; gi++)
        {
            auto glyphInterval = *gi;
            auto glyphEnd = glyphInterval >> 16;
            auto glyphBegin = glyphInterval ^ (glyphEnd << 16);

            if ((std::uint32_t)ch >= glyphBegin && (std::uint32_t)ch < glyphEnd)
                return glyphs[ofs + ch - glyphBegin];
            ofs += glyphEnd - glyphBegin;
        }

        return glyphs[0];
    }
};

