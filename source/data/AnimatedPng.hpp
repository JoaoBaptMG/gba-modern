//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Base class for the "naming convention" used by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include "BitmaskData.hpp"

struct AnimationFrame final
{
    std::int8_t dx, dy;
    std::uint16_t tileIdAttributes;
};

template <std::size_t DataCount, std::size_t ColorCount, std::size_t FrameCount, typename AnimationPoses>
struct AnimatedPng final
{
    struct __png
    {
        constexpr static const std::size_t NumAnimationFrames = FrameCount;
        std::uint8_t tiles[DataCount];
        std::uint16_t palette[ColorCount];
        AnimationFrame animationFrames[FrameCount];
        AnimationPoses poses{};
    } png;
};

template <std::size_t DataCount, std::size_t FrameCount, typename AnimationPoses>
struct AnimatedPng<DataCount, 0, FrameCount, AnimationPoses> final
{
    struct __png
    {
        constexpr static const std::size_t NumAnimationFrames = FrameCount;
        std::uint8_t tiles[DataCount];
        AnimationFrame animationFrames[FrameCount];
        AnimationPoses poses{};
    } png;
};

template <std::size_t DataCount, std::size_t ColorCount, std::size_t FrameCount, typename AnimationPoses>
struct AnimatedBitmaskPng final
{
    struct __png
    {
        constexpr static const std::size_t NumAnimationFrames = FrameCount;
        std::uint8_t tiles[DataCount];
        std::uint16_t palette[ColorCount];
        AnimationFrame animationFrames[FrameCount];
        BitmaskData bitmask;
        AnimationPoses poses{};
    } png;
};

template <std::size_t DataCount, std::size_t FrameCount, typename AnimationPoses>
struct AnimatedBitmaskPng<DataCount, 0, FrameCount, AnimationPoses> final
{
    struct __png
    {
        constexpr static const std::size_t NumAnimationFrames = FrameCount;
        std::uint8_t tiles[DataCount];
        AnimationFrame animationFrames[FrameCount];
        BitmaskData bitmask;
        AnimationPoses poses{};
    } png;
};
