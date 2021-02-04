//--------------------------------------------------------------------------------
// HudDefs.hpp
//--------------------------------------------------------------------------------
// Definitions used by both the Hud and the MsgBox classes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "data/sprites/hud.hpp"

#define HUD_TILE_BANK tile_mem[3]
#define HUD_SCREEN se_mem[31]
#define HUD_PALETTE pal_bg_bank[15]

namespace huddefs
{
    constexpr auto TilePos(int x, int y) { return 32 * y + x; }

    constexpr auto TileEnd = 512 - 2 * sizeof(SCREENBLOCK) / sizeof(TILE);

    constexpr auto NumScoreTiles = 5;
    constexpr auto ScoreTiles = TileEnd - NumScoreTiles;
    constexpr auto HudTiles = ScoreTiles - sizeof(data::sprites::hud.png.tiles)/sizeof(TILE);

    constexpr auto MsgBoxTileWidth = 25, MsgBoxTileHeight = 6;
    constexpr auto MsgBoxMaxOpenState = 28;
    constexpr auto NumMsgBoxTiles = MsgBoxTileWidth * MsgBoxTileHeight;
    constexpr auto MsgBoxTiles = HudTiles - NumMsgBoxTiles;

    constexpr auto PlayerHealthTileBase = TilePos(1, 1);
    constexpr auto ScoreTileBase = TilePos(25, 1);
    constexpr auto MsgBoxTileBase = TilePos(6, 15);
}