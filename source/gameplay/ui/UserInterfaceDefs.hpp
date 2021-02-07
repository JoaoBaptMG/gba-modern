//--------------------------------------------------------------------------------
// UserInterfaceDefs.hpp
//--------------------------------------------------------------------------------
// Definitions used by both the UserInterface and the MsgBox classes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <algorithm>
#include "data/sprites/user-interface.hpp"
#include "data/backgrounds/level-mark.hpp"

#include "data/backgrounds/level-numbers/_1.hpp"
#include "data/backgrounds/level-numbers/_2.hpp"
#include "data/backgrounds/level-numbers/_3.hpp"
#include "data/backgrounds/level-numbers/_4.hpp"
#include "data/backgrounds/level-numbers/_5.hpp"
#include "data/backgrounds/level-numbers/_6.hpp"
#include "data/backgrounds/level-numbers/_7.hpp"
#include "data/backgrounds/level-numbers/_8.hpp"
#include "data/backgrounds/level-numbers/_9.hpp"

constexpr auto getMaxNumberDataSize()
{
    using namespace data::backgrounds::level_numbers;
    return std::max({ 
        _1.png.CharDataSize,
        _2.png.CharDataSize,
        _3.png.CharDataSize,
        _4.png.CharDataSize,
        _5.png.CharDataSize,
        _6.png.CharDataSize,
        _7.png.CharDataSize,
        _8.png.CharDataSize,
        _9.png.CharDataSize,
    });
}

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

    constexpr auto NumLevelTextTiles = data::backgrounds::level_mark.png.CharDataSize/sizeof(TILE);
    constexpr auto NumLevelNumberTiles = getMaxNumberDataSize()/sizeof(TILE);
    constexpr auto NumLevelSignTiles = NumLevelTextTiles + NumLevelNumberTiles;

    constexpr auto LevelMarkerTileWidth = 20;
    constexpr auto LevelNumberTileWidth = 4;
    constexpr auto LevelSignTileSpacing = 1;
    constexpr auto LevelSignTileHeight = 6;

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