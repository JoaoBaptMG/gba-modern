//--------------------------------------------------------------------------------
// TextWriter.cpp
//--------------------------------------------------------------------------------
// Provides the base class for a text writer with a specific font
//--------------------------------------------------------------------------------
#include "TextWriter.hpp"

void TextWriter::write(int x, int y, const char* str, COLOR color)
{
    s16 px = x;
    int ch;
    while ((ch = *str++)) // This increments str until it is zero
    {
        if (ch >= font.glyphOffset)
        {
            std::size_t glyph = ch - font.glyphOffset;
            putGlyph(x, y, glyph, color);
            x += font.glyphs[glyph].advanceX;
        }
        else if (ch == '\n')
        {
            x = px;
            y += font.verticalStride;
        }
    }
}

void TextWriter::putChar(int x, int y, std::size_t ch, COLOR color)
{
    putGlyph(x, y, ch - font.glyphOffset, color);
}

void TextWriter::putGlyph(int x, int y, std::size_t glyph, COLOR color)
{
    // Add the real offset
    const auto& gl = font.glyphs[glyph];
    x += gl.offsetX;
    y += gl.offsetY;

    // Pointer to the screen 
    auto screen = &vid_mem[y*SCREEN_WIDTH + x];
    for (int i = 0; i < gl.height; i++)
    {
        auto row = gl.attr ? reinterpret_cast<const u32*>(gl.data)[i] :
            reinterpret_cast<const u16*>(gl.data)[i];

        for (int x = 0; row; row >>= 1, x++)
            if (row&1) screen[x] = color;

        screen += SCREEN_WIDTH;
    }
}