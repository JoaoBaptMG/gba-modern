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
    _8x8_4bpp, _16x16_4bpp, _32x32_4bpp, _64x64_4bpp,
    _16x8_4bpp, _32x16_4bpp, _32x8_4bpp, _64x32_4bpp,
    _8x16_4bpp, _16x32_4bpp, _8x32_4bpp, _32x64_4bpp,

    // 8bpp sizes
    _8x8_8bpp, _16x16_8bpp, _32x32_8bpp, _64x64_8bpp,
    _16x8_8bpp, _32x16_8bpp, _32x8_8bpp, _64x32_8bpp,
    _8x16_8bpp, _16x32_8bpp, _8x32_8bpp, _32x64_8bpp,
};

namespace SizeUtils
{
    constexpr u16 logBlocks(SpriteSize size)
    {
        switch (size)
        {
            // 4bpp sizes
            case SpriteSize::_8x8_4bpp: return 0;
            case SpriteSize::_16x16_4bpp: return 2;
            case SpriteSize::_32x32_4bpp: return 4;
            case SpriteSize::_64x64_4bpp: return 6;
            case SpriteSize::_16x8_4bpp: return 1;
            case SpriteSize::_32x16_4bpp: return 3;
            case SpriteSize::_32x8_4bpp: return 2;
            case SpriteSize::_64x32_4bpp: return 5;
            case SpriteSize::_8x16_4bpp: return 1;
            case SpriteSize::_16x32_4bpp: return 3;
            case SpriteSize::_8x32_4bpp: return 2;
            case SpriteSize::_32x64_4bpp: return 5;

            // 8bpp sizes
            case SpriteSize::_8x8_8bpp: return 1;
            case SpriteSize::_16x16_8bpp: return 3;
            case SpriteSize::_32x32_8bpp: return 5;
            case SpriteSize::_64x64_8bpp: return 7;
            case SpriteSize::_16x8_8bpp: return 2;
            case SpriteSize::_32x16_8bpp: return 4;
            case SpriteSize::_32x8_8bpp: return 3;
            case SpriteSize::_64x32_8bpp: return 6;
            case SpriteSize::_8x16_8bpp: return 2;
            case SpriteSize::_16x32_8bpp: return 4;
            case SpriteSize::_8x32_8bpp: return 3;
            case SpriteSize::_32x64_8bpp: return 6;
        }
        return 0;
    }

    constexpr u16 attr0param(SpriteSize size)
    {
        switch (size)
        {
            // 4bpp sizes
            case SpriteSize::_8x8_4bpp:
            case SpriteSize::_16x16_4bpp:
            case SpriteSize::_32x32_4bpp:
            case SpriteSize::_64x64_4bpp:
                return ATTR0_SQUARE | ATTR0_4BPP;

            case SpriteSize::_16x8_4bpp:
            case SpriteSize::_32x16_4bpp:
            case SpriteSize::_32x8_4bpp:
            case SpriteSize::_64x32_4bpp:
                return ATTR0_WIDE | ATTR0_4BPP;

            case SpriteSize::_8x16_4bpp:
            case SpriteSize::_16x32_4bpp:
            case SpriteSize::_8x32_4bpp:
            case SpriteSize::_32x64_4bpp:
                return ATTR0_TALL | ATTR0_4BPP;

            // 8bpp sizes
            case SpriteSize::_8x8_8bpp:
            case SpriteSize::_16x16_8bpp:
            case SpriteSize::_32x32_8bpp:
            case SpriteSize::_64x64_8bpp:
                return ATTR0_SQUARE | ATTR0_8BPP;

            case SpriteSize::_16x8_8bpp:
            case SpriteSize::_32x16_8bpp:
            case SpriteSize::_32x8_8bpp:
            case SpriteSize::_64x32_8bpp:
                return ATTR0_WIDE | ATTR0_8BPP;

            case SpriteSize::_8x16_8bpp:
            case SpriteSize::_16x32_8bpp:
            case SpriteSize::_8x32_8bpp:
            case SpriteSize::_32x64_8bpp:
                return ATTR0_TALL | ATTR0_8BPP;
        }
        return 0;
    }

    constexpr u16 attr1param(SpriteSize size)
    {
        switch (size)
        {
            case SpriteSize::_8x8_4bpp: case SpriteSize::_8x8_8bpp: return ATTR1_SIZE_8x8;
            case SpriteSize::_16x16_4bpp: case SpriteSize::_16x16_8bpp: return ATTR1_SIZE_16x16;
            case SpriteSize::_32x32_4bpp: case SpriteSize::_32x32_8bpp: return ATTR1_SIZE_32x32;
            case SpriteSize::_64x64_4bpp: case SpriteSize::_64x64_8bpp: return ATTR1_SIZE_64x64;
            case SpriteSize::_16x8_4bpp: case SpriteSize::_16x8_8bpp: return ATTR1_SIZE_16x8;
            case SpriteSize::_32x16_4bpp: case SpriteSize::_32x16_8bpp: return ATTR1_SIZE_32x16;
            case SpriteSize::_32x8_4bpp: case SpriteSize::_32x8_8bpp: return ATTR1_SIZE_32x8;
            case SpriteSize::_64x32_4bpp: case SpriteSize::_64x32_8bpp: return ATTR1_SIZE_64x32;
            case SpriteSize::_8x16_4bpp: case SpriteSize::_8x16_8bpp: return ATTR1_SIZE_8x16;
            case SpriteSize::_16x32_4bpp: case SpriteSize::_16x32_8bpp: return ATTR1_SIZE_16x32;
            case SpriteSize::_8x32_4bpp: case SpriteSize::_8x32_8bpp: return ATTR1_SIZE_8x32;
            case SpriteSize::_32x64_4bpp: case SpriteSize::_32x64_8bpp: return ATTR1_SIZE_32x64;
        }
        return 0;
    }
}
