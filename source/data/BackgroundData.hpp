//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Provides the TilesetData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>

namespace detail
{
    struct Empty1 {};
    struct Empty2 {};

    struct BackgroundDataCharsBase
    {
        const void* chars;
        const std::uint16_t* tiles;
    };

    template <std::size_t Size>
    struct BackgroundDataStaticChars : BackgroundDataCharsBase
    {
        constexpr static const std::size_t CharDataSize = Size;
    };

    struct BackgroundDataDynamicChars : BackgroundDataCharsBase
    {
        std::uint32_t charDataSize;
    };

    struct BackgroundDataTileSize
    {
        std::uint16_t tileWidth, tileHeight;
    };

    struct BackgroundDataPalettes
    {
        std::uint16_t numPalettes, is8bpp;
        const std::uint16_t* palettes;
    };
}

template <std::size_t StaticSize, bool ExportSizes, bool ExportPalettes>
struct BackgroundData final : std::conditional_t<StaticSize == (std::size_t)-1, 
        detail::BackgroundDataDynamicChars, detail::BackgroundDataStaticChars<StaticSize>>,
    std::conditional_t<ExportSizes, detail::BackgroundDataTileSize, detail::Empty1>,
    std::conditional_t<ExportPalettes, detail::BackgroundDataPalettes, detail::Empty2> {};

using FullBackgroundData = BackgroundData<(std::size_t)-1, true, true>;

template <std::size_t StaticSize, bool ExportSizes, bool ExportPalettes>
struct BackgroundHandle final
{
    BackgroundData<StaticSize, ExportSizes, ExportPalettes> png;
};
