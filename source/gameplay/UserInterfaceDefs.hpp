//--------------------------------------------------------------------------------
// UserInterfaceDefs.hpp
//--------------------------------------------------------------------------------
// Definitions used by both the UserInterface and the MsgBox classes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "data/sprites/user-interface.hpp"

#define UI_TILE_BANK tile_mem[3]
#define UI_SCREEN se_mem[31]
#define UI_PALETTE pal_bg_bank[15]

namespace uidefs
{
    constexpr auto TilePos(int x, int y) { return 32 * y + x; }

    constexpr auto TileEnd = 512 - 2 * sizeof(SCREENBLOCK) / sizeof(TILE);

    constexpr auto NumScoreTiles = 5;
    constexpr auto ScoreTiles = TileEnd - NumScoreTiles;
    constexpr auto UserInterfaceTiles = ScoreTiles - sizeof(data::sprites::user_interface.png.tiles)/sizeof(TILE);

    constexpr auto MsgBoxTileWidth = 25, MsgBoxTileHeight = 6;
    constexpr auto MsgBoxMaxOpenState = 28;
    constexpr auto NumMsgBoxTiles = MsgBoxTileWidth * MsgBoxTileHeight;
    constexpr auto MsgBoxTiles = UserInterfaceTiles - NumMsgBoxTiles;

    constexpr auto PlayerHealthTileBase = TilePos(1, 1);
    constexpr auto ScoreTileBase = TilePos(25, 1);
    constexpr auto MsgBoxTileBase = TilePos(6, 15);
}