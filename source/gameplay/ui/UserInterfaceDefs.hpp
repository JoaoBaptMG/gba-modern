//--------------------------------------------------------------------------------
// UserInterfaceDefs.hpp
//--------------------------------------------------------------------------------
// Definitions used by both the UserInterface and the MsgBox classes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <algorithm>
#include "data/sprites/user-interface.hpp"
#include "data/backgrounds/stage-mark.hpp"

#include "data/backgrounds/stage-numbers/_1.hpp"
#include "data/backgrounds/stage-numbers/_2.hpp"
#include "data/backgrounds/stage-numbers/_3.hpp"
#include "data/backgrounds/stage-numbers/_4.hpp"
#include "data/backgrounds/stage-numbers/_5.hpp"
#include "data/backgrounds/stage-numbers/_6.hpp"
#include "data/backgrounds/stage-numbers/_7.hpp"
#include "data/backgrounds/stage-numbers/_8.hpp"
#include "data/backgrounds/stage-numbers/_9.hpp"

constexpr auto getMaxNumberDataSize()
{
    return std::max({ 
        sizeof(data::backgrounds::stage_numbers::_1.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_2.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_3.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_4.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_5.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_6.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_7.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_8.png.tiles),
        sizeof(data::backgrounds::stage_numbers::_9.png.tiles),
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

    constexpr auto NumStageTextTiles = sizeof(data::backgrounds::stage_mark.png.tiles) / sizeof(TILE);
    constexpr auto NumStageNumberTiles = getMaxNumberDataSize()/sizeof(TILE);
    constexpr auto NumStageSignTiles = NumStageTextTiles + NumStageNumberTiles;

    constexpr auto StageMarkerTileWidth = data::backgrounds::stage_mark.png.SeWidth;
    constexpr auto StageNumberTileWidth = data::backgrounds::stage_numbers::_1.png.SeWidth;
    constexpr auto StageSignTileHeight = data::backgrounds::stage_mark.png.SeHeight;
    constexpr auto StageSignTileSpacing = 1;

    constexpr auto NumSignTiles = std::max(NumStageSignTiles, std::size_t());
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