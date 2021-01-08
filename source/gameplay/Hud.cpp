//--------------------------------------------------------------------------------
// Hud.cpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#include "Hud.hpp"

#include "GameScene.hpp"
#include "data/sprites/hud.hpp"
#include "data/fonts/monogram_extended.hpp"
#include "util/uintDigits.h"

constexpr auto TileEnd = 512 - 2 * sizeof(SCREENBLOCK) / sizeof(TILE);

constexpr auto NumScoreTiles = 5;
constexpr auto ScoreTiles = TileEnd - NumScoreTiles;
constexpr auto HudTiles = ScoreTiles - sizeof(data::sprites::hud.png.tiles)/sizeof(TILE);

constexpr auto ScoreTileBase = 32 * 1 + 25;

Hud::Hud() : scoreWriter(data::fonts::monogram_extended.ttf, &tile_mem[3][ScoreTiles], 8) {}

void Hud::init()
{
    // Initialize BG0
    REG_BG0CNT = BG_CBB(3) | BG_SBB(31) | BG_REG_32x32;

    // Move BG0 half a tile
    REG_BG0HOFS = 4;
    REG_BG0VOFS = 4;

    // Transfer the data to the end of the CBB
    constexpr auto DataSize = sizeof(data::sprites::hud.png.tiles);
    memcpy32(&tile_mem[3][HudTiles], data::sprites::hud.png.tiles, DataSize/sizeof(u32));

    // And set the SBB to the correct place
    constexpr auto TileId = SE_PALBANK(15) | (HudTiles - 1);
    memset32(&se_mem[31], TileId | (TileId << 16), sizeof(SCREENBLOCK)/sizeof(u32));

    // Transfer the palette
    memcpy32(&pal_bg_bank[15], data::sprites::hud.png.palette, sizeof(PALBANK)/sizeof(u32));

    // Set the score SBB to the correct place
    for (u32 i = 0; i < NumScoreTiles; i++)
        se_mem[31][ScoreTileBase + i] = SE_PALBANK(15) | (ScoreTiles + i);

    // Set the two graphic effects
    clearAlpha = 0;
    reloadAlpha = 12;

    REG_BLDY = reloadAlpha;
}

void Hud::vblank()
{
    // Set the tiles here
    constexpr auto TileBase = 33;
    const Player& player = gameScene().player;

    int i;
    for (i = 0; i < player.getHealth(); i++)
        se_mem[31][TileBase+i] = SE_PALBANK(15) | HudTiles;
    for (; i < player.getMaxHealth(); i++)
        se_mem[31][TileBase+i] = SE_PALBANK(15) | (HudTiles + 1);
    se_mem[31][TileBase+i] = SE_PALBANK(15) | (HudTiles + 2);

    // Reset the alpha blending
    REG_BLDCNT = BLD_TOP(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_BLACK;

    // Set the two graphic effects
    clearAlpha = 0;
    reloadAlpha = 12;
}

void Hud::update()
{
    // Add the two effects
    graphics::hblankEffects.add16(HudSize, &clearAlpha, (void*)&REG_BLDY, 1);
    graphics::hblankEffects.add16(SCREEN_HEIGHT - HudSize, &reloadAlpha, (void*)&REG_BLDY, 1);

    // Clear the score text
    memset32(&tile_mem[3][ScoreTiles], 0, NumScoreTiles*sizeof(TILE)/sizeof(u32));

    // Create the text
    char str[] = "      ";
    char vals[6];
    auto end = uintDigits(vals, gameScene().projectiles.getTotalNumProjectiles());
    for (int i = 0; i < end-vals; i++) str[5-i] = '0' + vals[i];

    // And write it
    scoreWriter.write(0, 8, str, 8);
}

GameScene& Hud::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, hud));
#pragma GCC diagnostic pop
}
