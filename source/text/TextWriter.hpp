//--------------------------------------------------------------------------------
// TextWriter.hpp
//--------------------------------------------------------------------------------
// Provides the base class for a text writer with a specific font
//--------------------------------------------------------------------------------
#pragma once

#include "data/Font.hpp"
#include <tonc.h>

class TextWriter final
{
    const Font& font;

    void putGlyph(int x, int y, std::size_t glyph, COLOR color);

public:
    TextWriter(const Font& font) : font(font) {}

    // Put an entire string in the screen
    void write(int x, int y, const char* str, COLOR color);

    // Put a single char in the screen
    void putChar(int x, int y, std::size_t ch, COLOR color);
};
