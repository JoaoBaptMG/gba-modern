//--------------------------------------------------------------------------------
// UserInterfaceDefs.hpp
//--------------------------------------------------------------------------------
// Definitions used by both the UserInterface and the MsgBox classes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <algorithm>
#include "data/sprites/user-interface.hpp"
#include "data/sprites/level-marker.hpp"
#include "data/sprites/level-numbers.hpp"

#define UI_TILE_BANK tile_mem[3]
#define UI_SCREEN se_mem[31]
#define UI_PALETTE pal_bg_bank[15]
#define UI_SIGN_PALETTE pal_bg_bank[14]

namespace uidefs
{
    constexpr auto TilePos(int x, int y) { return 32 * y + x; }

    constexpr auto TileEnd = 512 - 2 * sizeof(SCREENBLOCK) / sizeof(TILE);

    constexpr auto NumScoreTiles = 5;
    constexpr auto ScoreTiles = TileEnd - NumScoreTiles;

    constexpr auto NumLevelTextTiles = sizeof(data::sprites::level_marker.png.tiles)/sizeof(TILE);
    constexpr auto NumLevelNumberTiles = sizeof(data::sprites::level_numbers.png.tiles)/sizeof(TILE) / 9;
    constexpr auto NumLevelSignTiles = NumLevelTextTiles + NumLevelNumberTiles;

    constexpr auto LevelMarkerTileWidth = 22;
    constexpr auto LevelNumberTileWidth = 5;
    constexpr auto LevelSignTileHeight = 7;

    constexpr auto NumSignTiles = std::max(NumLevelSignTiles, std::size_t());
    constexpr auto SignTiles = ScoreTiles - NumSignTiles;

    constexpr auto MsgBoxTileWidth = 25, MsgBoxTileHeight = 6;
    constexpr auto MsgBoxMaxOpenState = 28;
    constexpr auto NumMsgBoxTiles = MsgBoxTileWidth * MsgBoxTileHeight;
    constexpr auto MsgBoxTiles = SignTiles - NumMsgBoxTiles;

    constexpr auto UserInterfaceTiles = MsgBoxTiles - sizeof(data::sprites::user_interface.png.tiles)/sizeof(TILE);

    constexpr auto PlayerHealthTileBase = TilePos(1, 1);
    constexpr auto ScoreTileBase = TilePos(25, 1);
    constexpr auto MsgBoxTileBase = TilePos(6, 15);
    constexpr auto SignTileBase = TilePos(0, 7);
}