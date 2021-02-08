//--------------------------------------------------------------------------------
// AnimatedPng.hpp
//--------------------------------------------------------------------------------
// Provides the TilesetData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>
#include <tonc.h>

namespace detail::background
{
    template <std::size_t DataSize, bool _Is8bpp>
    struct Tiles
    {
        static constexpr const bool Is8bpp = _Is8bpp;
        u8 tiles[DataSize];
    };
    template <bool Is8bpp> struct Tiles<0, Is8bpp> {};

    template <std::size_t _SeWidth, std::size_t _SeHeight>
    struct ScreenEntries
    {
        static constexpr const std::size_t SeWidth = _SeWidth;
        static constexpr const std::size_t SeHeight = _SeHeight;
        SCR_ENTRY scrEntries[_SeWidth*_SeHeight];
        auto getEntry(u32 x, u32 y) const { return scrEntries[y*_SeWidth + x]; }
    };
    template <std::size_t Dummy> struct ScreenEntries<Dummy, 0> {};
    template <std::size_t Dummy> struct ScreenEntries<0, Dummy> {};
    template <> struct ScreenEntries<0, 0> {};

    template <std::size_t PaletteCount>
    struct Palettes { PALBANK palettes[PaletteCount]; };
    template <> struct Palettes<0> {};

    template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
    struct BackgroundData final : Tiles<DataSize, Is8bpp>, ScreenEntries<SeWidth, SeHeight>, Palettes<PaletteCount> {};
}

using detail::background::BackgroundData;

template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
struct BackgroundHandle final
{
    BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, PaletteCount> png;
};

namespace data
{
    template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
    std::enable_if_t<DataSize != 0> 
    copyTiles(void* dest, const BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, PaletteCount>& background)
    {
        memcpy32(dest, background.tiles, sizeof(background.tiles)/sizeof(u32));
    }

    template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
    std::enable_if_t<SeWidth != 0 && SeHeight != 0> 
    copyScrEntries(void* dest, const BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, PaletteCount>& background)
    {
        memcpy32(dest, background.scrEntries, sizeof(background.scrEntries)/sizeof(u32));
    }

    template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
    std::enable_if_t<PaletteCount != 0>
    copyPalettes(void* dest, const BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, PaletteCount>& background)
    {
        memcpy32(dest, background.palettes, sizeof(background.palettes)/sizeof(u32));
    }
}
