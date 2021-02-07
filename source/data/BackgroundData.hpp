//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Provides the TilesetData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>
#include <tonc_types.h>

namespace detail
{
    struct Empty1 {};
    struct Empty2 {};

    struct BackgroundDataTilesBase
    {
        const void* tiles;
        const SCR_ENTRY* scrEntries;
    };

    template <std::size_t Size>
    struct BackgroundDataStaticTiles : BackgroundDataTilesBase
    {
        constexpr static const std::size_t CharDataSize = Size;
    };

    struct BackgroundDataDynamicTiles : BackgroundDataTilesBase
    {
        std::uint32_t charDataSize;
    };

    struct BackgroundDataScrSize
    {
        std::uint16_t seWidth, seHeight;
    };

    struct BackgroundDataPalettes
    {
        std::uint16_t numPalettes, is8bpp;
        const std::uint16_t* palettes;
    };
}

template <std::size_t StaticSize, bool ExportSizes, bool ExportPalettes>
struct BackgroundData final : std::conditional_t<StaticSize == (std::size_t)-1, 
        detail::BackgroundDataDynamicTiles, detail::BackgroundDataStaticTiles<StaticSize>>,
    std::conditional_t<ExportSizes, detail::BackgroundDataScrSize, detail::Empty1>,
    std::conditional_t<ExportPalettes, detail::BackgroundDataPalettes, detail::Empty2> {};

using FullBackgroundData = BackgroundData<(std::size_t)-1, true, true>;

template <std::size_t StaticSize, bool ExportSizes, bool ExportPalettes>
struct BackgroundHandle final
{
    BackgroundData<StaticSize, ExportSizes, ExportPalettes> png;
};
