//--------------------------------------------------------------------------------
// Mode3GlyphWriter.hpp
//--------------------------------------------------------------------------------
// Provides a way to write glyphs in text for a mode-3 setting
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "../data/Font.hpp"

extern "C" void mode3PutGlyph(int x, int y, const GlyphData* gl, COLOR color, void* buffer);

class Mode3GlyphWriter final
{
    COLOR* buffer;

public:
    Mode3GlyphWriter(COLOR* buffer) : buffer(buffer) {}

    void putGlyph(int x, int y, const GlyphData& gl, COLOR color)
    {
        mode3PutGlyph(x, y, &gl, color, buffer);
    }
};
