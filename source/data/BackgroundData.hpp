//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Provides the TilesetData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>
#include <tonc_types.h>

namespace detail::background
{
    template <std::size_t _DataSize, bool _Is8bpp>
    struct Data
    {
        static constexpr const bool Is8bpp = _Is8bpp;
        static constexpr const std::size_t DataSize = _DataSize;
        u8 tiles[_DataSize];
    };
    template <bool Is8bpp> struct Data<0, Is8bpp> {};

    template <std::size_t _SeWidth, std::size_t _SeHeight>
    struct ScreenEntries
    {
        static constexpr const std::size_t SeWidth = _SeWidth;
        static constexpr const std::size_t SeHeight = _SeHeight;
        SCR_ENTRY scrEntries[_SeWidth*_SeHeight];
        auto getEntry(std::size_t x, std::size_t y) const
        { return scrEntries[y*_SeWidth + x]; }
    };
    template <std::size_t Dummy> struct ScreenEntries<Dummy, 0> {};
    template <std::size_t Dummy> struct ScreenEntries<0, Dummy> {};
    template <> struct ScreenEntries<0, 0> {};

    template <std::size_t PaletteCount>
    struct Palettes { PALBANK palettes[PaletteCount]; };
    template <> struct Palettes<0> {};

    template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
    struct BackgroundData final : Data<DataSize, Is8bpp>, ScreenEntries<SeWidth, SeHeight>, Palettes<PaletteCount> {};
}

using detail::background::BackgroundData;

template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
struct BackgroundHandle final
{
    BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, PaletteCount> png;
};
