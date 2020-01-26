//--------------------------------------------------------------------------------
// TextWriter.hpp
//--------------------------------------------------------------------------------
// Provides the base class for a text writer with a specific font
//--------------------------------------------------------------------------------
#pragma once

#include "data/Font.hpp"
#include <tonc.h>
#include "StringBuilder.hpp"

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
        int ch;
        while ((ch = *str++)) // This increments str until it is zero
        {
            if (ch >= font.glyphOffset)
            {
                const GlyphData& glyph = font.glyphFor(ch);
                glyphWriter.putGlyph(x, y, glyph, color);
                x += glyph.advanceX;
            }
            else if (ch == '\n')
            {
                x = px;
                y += font.verticalStride;
            }
        }
    }

    // Quick shortcut for StringBuilder
    template <std::size_t N>
    void write(int x, int y, StringBuilder<N>& sb, COLOR color)
    {
        auto str = sb.getString();
        write(x, y, str.data(), color);
    }

    // Put a single char in the screen
    void putChar(int x, int y, std::size_t ch, COLOR color)
    {
        glyphWriter.putGlyph(x, y, font.glyphFor(ch), color);
    }

    GlyphWriter glyphWriter;
};
