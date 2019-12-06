//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Base class for the "naming convention" used by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

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
        const AnimationPoses poses{};
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
        const AnimationPoses poses{};
    } png;
};
