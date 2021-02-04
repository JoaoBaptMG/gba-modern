//--------------------------------------------------------------------------------
// Hud.cpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#include "Hud.hpp"

#include "GameScene.hpp"
#include "data/fonts/monogram_extended.hpp"
#include "util/uintDigits.h"

#include "HudDefs.hpp"

Hud::Hud() : scoreWriter(data::fonts::monogram_extended.ttf, &HUD_TILE_BANK[huddefs::ScoreTiles], 8) {}

void Hud::init()
{
    // Initialize BG0
    REG_BG0CNT = BG_CBB(3) | BG_SBB(31) | BG_REG_32x32;

    // Move BG0 half a tile
    REG_BG0HOFS = 4;
    REG_BG0VOFS = 4;

    // Transfer the data to the end of the CBB
    constexpr auto DataSize = sizeof(data::sprites::hud.png.tiles);
    memcpy32(&HUD_TILE_BANK[huddefs::HudTiles], data::sprites::hud.png.tiles, DataSize/sizeof(u32));

    // And set the SBB to the correct place
    constexpr auto TileId = SE_PALBANK(15) | (huddefs::HudTiles - 1);
    memset32(&HUD_SCREEN, TileId | (TileId << 16), sizeof(SCREENBLOCK)/sizeof(u32));

    // Transfer the palette
    memcpy32(&HUD_PALETTE, data::sprites::hud.png.palette, sizeof(PALBANK)/sizeof(u32));

    // Set the score SBB to the correct place
    for (u32 i = 0; i < huddefs::NumScoreTiles; i++)
        HUD_SCREEN[huddefs::ScoreTileBase + i] = SE_PALBANK(15) | (huddefs::ScoreTiles + i);

    // Set the two graphic effects
    clearAlpha = 0;
    reloadAlpha = 12;

    REG_BLDY = reloadAlpha;
}

void Hud::vblank()
{
    // Set the tiles here
    constexpr auto TileBase = huddefs::PlayerHealthTileBase;
    const Player& player = gameScene().player;

    int i;
    for (i = 0; i < player.getHealth(); i++)
        HUD_SCREEN[TileBase+i] = SE_PALBANK(15) | huddefs::HudTiles;
    for (; i < player.getMaxHealth(); i++)
        HUD_SCREEN[TileBase+i] = SE_PALBANK(15) | (huddefs::HudTiles + 1);
    HUD_SCREEN[TileBase+i] = SE_PALBANK(15) | (huddefs::HudTiles + 2);

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
    graphics::hblankEffects.add16(SCREEN_HEIGHT - HudSize - msgBox.openState,
        &reloadAlpha, (void*)&REG_BLDY, 1);

    // Clear the score text
    memset32(&HUD_TILE_BANK[huddefs::ScoreTiles], 0, huddefs::NumScoreTiles * sizeof(TILE)/sizeof(u32));

    // Create the text
    char str[] = "      ";
    char vals[6];
    auto end = uintDigits(vals, gameScene().projectiles.getTotalNumProjectiles());
    for (int i = 0; i < end-vals; i++) str[5-i] = '0' + vals[i];

    // And write it
    scoreWriter.write(5, 8, str, 8);

    msgBox.update();
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
