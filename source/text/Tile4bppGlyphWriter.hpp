//--------------------------------------------------------------------------------
// Tile4bppGlyphWriter.hpp
//--------------------------------------------------------------------------------
// Provides a way to write glyphs in VRAM for a correctly tilemapped region
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "../data/Font.hpp"

extern "C" void tile4bppPutGlyph(int x, int y, const GlyphData* gl, COLOR color, void* buffer, u32 pixelHeight);

class Tile4bppGlyphWriter final
{
    // The drawing area
    void* buffer;

    // The height of the drawing area IN PIXELS (aka 8 * in tile)
    u32 pixelHeight;

public:
    Tile4bppGlyphWriter(void* buffer, u32 pixelHeight) : buffer(buffer), pixelHeight(pixelHeight) {}

    void putGlyph(int x, int y, const GlyphData& gl, COLOR color)
    {
        tile4bppPutGlyph(x, y, &gl, color, buffer, pixelHeight);
    }
};
