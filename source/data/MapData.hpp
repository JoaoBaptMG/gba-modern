//--------------------------------------------------------------------------------
// MapData.hpp
//--------------------------------------------------------------------------------
// Provides the MapData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "TilesetData.hpp"

class GameScene;

struct alignas(u32) MapData final
{
    u16 width, height;
    const TilesetData* tileset;
    const u8* data;
    void (*initFunction)(GameScene&);

    constexpr auto tile(u32 x, u32 y) const
    {
        return data[y*width+x];
    }

    constexpr const auto& tileGraphics(u32 x, u32 y) const
    {
        return tileset->tiles[tile(x, y)];
    }
};
