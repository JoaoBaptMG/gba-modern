//--------------------------------------------------------------------------------
// SpriteSize.hpp
//--------------------------------------------------------------------------------
// An enum for the sprite size, used in the buddy object allocator and in the
// animator classes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "../math/vec2.hpp"

// Sprite size enum
enum class SpriteSize : u8
{
    // 4bpp sizes
    s8x8_4bpp, s16x16_4bpp, s32x32_4bpp, s64x64_4bpp,
    s16x8_4bpp, s32x16_4bpp, s32x8_4bpp, s64x32_4bpp,
    s8x16_4bpp, s16x32_4bpp, s8x32_4bpp, s32x64_4bpp,

    // 8bpp sizes
    s8x8_8bpp, s16x16_8bpp, s32x32_8bpp, s64x64_8bpp,
    s16x8_8bpp, s32x16_8bpp, s32x8_8bpp, s64x32_8bpp,
    s8x16_8bpp, s16x32_8bpp, s8x32_8bpp, s32x64_8bpp,
};

namespace SizeUtils
{
    constexpr u16 logBlocks(SpriteSize size)
    {
        switch (size)
        {
            // 4bpp sizes
            case SpriteSize::s8x8_4bpp: return 0;
            case SpriteSize::s16x16_4bpp: return 2;
            case SpriteSize::s32x32_4bpp: return 4;
            case SpriteSize::s64x64_4bpp: return 6;
            case SpriteSize::s16x8_4bpp: return 1;
            case SpriteSize::s32x16_4bpp: return 3;
            case SpriteSize::s32x8_4bpp: return 2;
            case SpriteSize::s64x32_4bpp: return 5;
            case SpriteSize::s8x16_4bpp: return 1;
            case SpriteSize::s16x32_4bpp: return 3;
            case SpriteSize::s8x32_4bpp: return 2;
            case SpriteSize::s32x64_4bpp: return 5;

            // 8bpp sizes
            case SpriteSize::s8x8_8bpp: return 1;
            case SpriteSize::s16x16_8bpp: return 3;
            case SpriteSize::s32x32_8bpp: return 5;
            case SpriteSize::s64x64_8bpp: return 7;
            case SpriteSize::s16x8_8bpp: return 2;
            case SpriteSize::s32x16_8bpp: return 4;
            case SpriteSize::s32x8_8bpp: return 3;
            case SpriteSize::s64x32_8bpp: return 6;
            case SpriteSize::s8x16_8bpp: return 2;
            case SpriteSize::s16x32_8bpp: return 4;
            case SpriteSize::s8x32_8bpp: return 3;
            case SpriteSize::s32x64_8bpp: return 6;
        }
        return 0;
    }

    constexpr u16 attr0param(SpriteSize size)
    {
        switch (size)
        {
            // 4bpp sizes
            case SpriteSize::s8x8_4bpp:
            case SpriteSize::s16x16_4bpp:
            case SpriteSize::s32x32_4bpp:
            case SpriteSize::s64x64_4bpp:
                return ATTR0_SQUARE | ATTR0_4BPP;

            case SpriteSize::s16x8_4bpp:
            case SpriteSize::s32x16_4bpp:
            case SpriteSize::s32x8_4bpp:
            case SpriteSize::s64x32_4bpp:
                return ATTR0_WIDE | ATTR0_4BPP;

            case SpriteSize::s8x16_4bpp:
            case SpriteSize::s16x32_4bpp:
            case SpriteSize::s8x32_4bpp:
            case SpriteSize::s32x64_4bpp:
                return ATTR0_TALL | ATTR0_4BPP;

            // 8bpp sizes
            case SpriteSize::s8x8_8bpp:
            case SpriteSize::s16x16_8bpp:
            case SpriteSize::s32x32_8bpp:
            case SpriteSize::s64x64_8bpp:
                return ATTR0_SQUARE | ATTR0_8BPP;

            case SpriteSize::s16x8_8bpp:
            case SpriteSize::s32x16_8bpp:
            case SpriteSize::s32x8_8bpp:
            case SpriteSize::s64x32_8bpp:
                return ATTR0_WIDE | ATTR0_8BPP;

            case SpriteSize::s8x16_8bpp:
            case SpriteSize::s16x32_8bpp:
            case SpriteSize::s8x32_8bpp:
            case SpriteSize::s32x64_8bpp:
                return ATTR0_TALL | ATTR0_8BPP;
        }
        return 0;
    }

    constexpr u16 attr1param(SpriteSize size)
    {
        switch (size)
        {
            case SpriteSize::s8x8_4bpp: case SpriteSize::s8x8_8bpp: return ATTR1_SIZE_8x8;
            case SpriteSize::s16x16_4bpp: case SpriteSize::s16x16_8bpp: return ATTR1_SIZE_16x16;
            case SpriteSize::s32x32_4bpp: case SpriteSize::s32x32_8bpp: return ATTR1_SIZE_32x32;
            case SpriteSize::s64x64_4bpp: case SpriteSize::s64x64_8bpp: return ATTR1_SIZE_64x64;
            case SpriteSize::s16x8_4bpp: case SpriteSize::s16x8_8bpp: return ATTR1_SIZE_16x8;
            case SpriteSize::s32x16_4bpp: case SpriteSize::s32x16_8bpp: return ATTR1_SIZE_32x16;
            case SpriteSize::s32x8_4bpp: case SpriteSize::s32x8_8bpp: return ATTR1_SIZE_32x8;
            case SpriteSize::s64x32_4bpp: case SpriteSize::s64x32_8bpp: return ATTR1_SIZE_64x32;
            case SpriteSize::s8x16_4bpp: case SpriteSize::s8x16_8bpp: return ATTR1_SIZE_8x16;
            case SpriteSize::s16x32_4bpp: case SpriteSize::s16x32_8bpp: return ATTR1_SIZE_16x32;
            case SpriteSize::s8x32_4bpp: case SpriteSize::s8x32_8bpp: return ATTR1_SIZE_8x32;
            case SpriteSize::s32x64_4bpp: case SpriteSize::s32x64_8bpp: return ATTR1_SIZE_32x64;
        }
        return 0;
    }

    constexpr vec2<int> pixelSize(SpriteSize size)
    {
        switch (size)
        {
            case SpriteSize::s8x8_4bpp: case SpriteSize::s8x8_8bpp: return vec2(8, 8);
            case SpriteSize::s16x16_4bpp: case SpriteSize::s16x16_8bpp: return vec2(16, 16);
            case SpriteSize::s32x32_4bpp: case SpriteSize::s32x32_8bpp: return vec2(32, 32);
            case SpriteSize::s64x64_4bpp: case SpriteSize::s64x64_8bpp: return vec2(64, 64);
            case SpriteSize::s16x8_4bpp: case SpriteSize::s16x8_8bpp: return vec2(16, 8);
            case SpriteSize::s32x16_4bpp: case SpriteSize::s32x16_8bpp: return vec2(32, 16);
            case SpriteSize::s32x8_4bpp: case SpriteSize::s32x8_8bpp: return vec2(32, 8);
            case SpriteSize::s64x32_4bpp: case SpriteSize::s64x32_8bpp: return vec2(64, 32);
            case SpriteSize::s8x16_4bpp: case SpriteSize::s8x16_8bpp: return vec2(8, 16);
            case SpriteSize::s16x32_4bpp: case SpriteSize::s16x32_8bpp: return vec2(16, 32);
            case SpriteSize::s8x32_4bpp: case SpriteSize::s8x32_8bpp: return vec2(8, 32);
            case SpriteSize::s32x64_4bpp: case SpriteSize::s32x64_8bpp: return vec2(32, 64);
        }
        return vec2<int>();
    }
}
