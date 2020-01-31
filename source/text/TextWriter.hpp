//--------------------------------------------------------------------------------
// TextWriter.hpp
//--------------------------------------------------------------------------------
// Provides the base class for a text writer with a specific font
//--------------------------------------------------------------------------------
#pragma once

#include "data/Font.hpp"
#include <tonc.h>
#include "StringBuilder.hpp"

extern "C" std::uint64_t decodeUtf8Asm(const char *str);

inline static u32 decodeUtf8(const char*& str)
{
    union
    { 
        std::uint64_t res;
        struct { const char* newstr; u32 ch; } vals;
    } result;

    result.res = decodeUtf8Asm(str);
    str = result.vals.newstr;
    return result.vals.ch;
}

template <typename GlyphWriter>
class TextWriter final
{
    const Font& font;

public:
    template <typename... Ts>
    TextWriter(const Font& font, Ts&&... vs)
        : font(font), glyphWriter(std::forward<Ts>(vs)...) {}

    // Put an entire string in the screen
    void write(int x, int y, const char* str, COLOR color)
    {
        s16 px = x;
        u32 ch;
        while ((ch = decodeUtf8(str))) // This increments str until it is zero
        {
            if (ch == '\n')
            {
                x = px;
                y += font.verticalStride;
            }
            else
            {
                const GlyphData& glyph = font.glyphFor(ch);
                glyphWriter.putGlyph(x, y, glyph, color);
                x += glyph.advanceX;
            }
        }
    }

    // Quick shortcut for StringBuilder
    template <std::size_t N>
    void write(int x, int y, StringBuilder<N>& sb, COLOR color)
    {
        write(x, y, sb.getString(), color);
    }

    // Put a single char in the screen
    void putChar(int x, int y, u32 ch, COLOR color)
    {
        glyphWriter.putGlyph(x, y, font.glyphFor(ch), color);
    }

    GlyphWriter glyphWriter;
};
