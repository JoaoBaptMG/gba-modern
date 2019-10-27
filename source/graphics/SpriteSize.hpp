//--------------------------------------------------------------------------------
// SpriteSize.hpp
//--------------------------------------------------------------------------------
// An enum for the sprite size, used in the buddy object allocator and in the
// animator classes
//--------------------------------------------------------------------------------

#pragma once

#include <tonc.h>

// Sprite size enum
enum class SpriteSize : u16
{
    // 4bpp sizes
    _8x8_4bpp = 0, _16x16_4bpp = 2, _32x32_4bpp = 4, _64x64_4bpp = 6,
    _16x8_4bpp = 1, _32x16_4bpp = 3, _32x8_4bpp = 2, _64x32_4bpp = 5,
    _8x16_4bpp = 1, _16x32_4bpp = 3, _8x32_4bpp = 2, _32x64_4bpp = 5,

    // 8bpp sizes
    _8x8_8bpp = 1, _16x16_8bpp = 3, _32x32_8bpp = 5, _64x64_8bpp = 7,
    _16x8_8bpp = 2, _32x16_8bpp = 4, _32x8_8bpp = 3, _64x32_8bpp = 6,
    _8x16_8bpp = 2, _16x32_8bpp = 4, _8x32_8bpp = 3, _32x64_8bpp = 6,
};
