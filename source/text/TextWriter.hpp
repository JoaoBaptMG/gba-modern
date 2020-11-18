//--------------------------------------------------------------------------------
// TextWriter.hpp
//--------------------------------------------------------------------------------
// Provides the base class for a text writer with a specific font
//--------------------------------------------------------------------------------
#pragma once

#include "../data/Font.hpp"
#include <tonc.h>
#include "StringBuilder.hpp"

inline static u32 decodeUtf8(const char*& str)
{
    u32 ch = *str++;
    if (!(ch & 0x80)) return ch;
    ch = (ch << 6) | (*str++ & 0x3F);
    if (!(ch & (1 << 11))) return ch & ~(3 << 12);
    return (ch << 6) | (*str++ & 0x3F);
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
