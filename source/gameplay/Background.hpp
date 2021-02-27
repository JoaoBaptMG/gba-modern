//--------------------------------------------------------------------------------
// Background.hpp
//--------------------------------------------------------------------------------
// Class for showing a background
//--------------------------------------------------------------------------------
#pragma once

#include "data/BackgroundData.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class GameScene;

struct DynamicBackgroundData
{
    u32 charDataSize;
    u16 seWidth, seHeight, numPalettes, is8bpp;
    const void* tiles;
    const SCR_ENTRY* scrEntries;
    const PALBANK* palettes;
};

template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
DynamicBackgroundData getDynamicBackgroundData(
    const BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, false, PaletteCount>& background)
{
    return { DataSize, SeWidth, SeHeight, PaletteCount, Is8bpp,
        background.tiles, background.scrEntries, background.palettes };
}

class Background final
{
    GameScene& gameScene();
    void load(const DynamicBackgroundData& background);

public:
    Background() {}
    void init();
    void vblank();

    template <std::size_t DataSize, bool Is8bpp, std::size_t SeWidth, std::size_t SeHeight, std::size_t PaletteCount>
    void load(const BackgroundData<DataSize, Is8bpp, SeWidth, SeHeight, false, PaletteCount>& background)
    { load(getDynamicBackgroundData(background)); }

    vec2<s32f8> offset;
};
