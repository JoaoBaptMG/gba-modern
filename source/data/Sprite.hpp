//--------------------------------------------------------------------------------
// Sprite.hpp
//--------------------------------------------------------------------------------
// Provides a data structure to manipulate the sprite data
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <tonc.h>
#include <type_traits>
#include "BitmaskData.hpp"
#include "AnimationFrame.hpp"

namespace detail::sprite
{
    template <std::size_t DataSize>
    struct Tiles { u8 tiles[DataSize]; };
    template <> struct Tiles<0> {};

    template <std::size_t ColorCount>
    struct Palette { u16 palette[ColorCount]; };
    template <> struct Palette<0> {};

    template <std::size_t BitmaskStructSize>
    struct Bitmask
    { 
        BitmaskData bitmask;
        u16 __actualBitmaskData[BitmaskStructSize];
    };
    template <> struct Bitmask<0> {};

    template <std::size_t FrameCount, typename AnimationPoses>
    struct Animation
    {
        constexpr static const std::size_t NumAnimationFrames = FrameCount;
        AnimationFrame animationFrames[FrameCount];
        AnimationPoses poses{};
    };

    template <typename AnimationPoses>
    struct Animation<0, AnimationPoses> {};

    template <std::size_t DataSize, std::size_t ColorCount, std::size_t BitmaskStructSize,
        std::size_t FrameCount = 0, typename AnimationPoses = void>
    struct Sprite final : Tiles<DataSize>, Palette<ColorCount>, Bitmask<BitmaskStructSize>,
        Animation<FrameCount, AnimationPoses> {};
};

using detail::sprite::Sprite;

template <std::size_t DataSize, std::size_t ColorCount, std::size_t BitmaskStructSize,
    std::size_t FrameCount = 0, typename AnimationPoses = void>
struct SpriteHandle final
{
    Sprite<DataSize, ColorCount, BitmaskStructSize, FrameCount, AnimationPoses> png;
};

namespace data
{
    template <std::size_t DataSize, std::size_t ColorCount, std::size_t BitmaskStructSize,
        std::size_t FrameCount, typename AnimationPoses>
    std::enable_if_t<DataSize != 0> 
    copyTiles(void* dest, const Sprite<DataSize, ColorCount, BitmaskStructSize, FrameCount, AnimationPoses>& sprite)
    {
        memcpy32(dest, sprite.tiles, sizeof(sprite.tiles)/sizeof(u32));
    }

    template <std::size_t DataSize, std::size_t ColorCount, std::size_t BitmaskStructSize,
        std::size_t FrameCount, typename AnimationPoses>
    std::enable_if_t<ColorCount != 0> 
    copyPalette(void* dest, const Sprite<DataSize, ColorCount, BitmaskStructSize, FrameCount, AnimationPoses>& sprite)
    {
        memcpy32(dest, sprite.palette, (sizeof(sprite.palette)+sizeof(u32)-1)/sizeof(u32));
    }
}
