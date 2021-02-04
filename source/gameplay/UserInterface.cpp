//--------------------------------------------------------------------------------
// UserInterface.cpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#include "UserInterface.hpp"

#include "GameScene.hpp"
#include "data/fonts/monogram_extended.hpp"
#include "util/uintDigits.h"

#include "UserInterfaceDefs.hpp"

UserInterface::UserInterface() : scoreWriter(data::fonts::monogram_extended.ttf, &UI_TILE_BANK[uidefs::ScoreTiles], 8) {}

void UserInterface::init()
{
    // Initialize BG0
    REG_BG0CNT = BG_CBB(3) | BG_SBB(31) | BG_REG_32x32;

    // Move BG0 half a tile
    REG_BG0HOFS = 4;
    REG_BG0VOFS = 4;

    // Transfer the data to the end of the CBB
    constexpr auto DataSize = sizeof(data::sprites::user_interface.png.tiles);
    memcpy32(&UI_TILE_BANK[uidefs::UserInterfaceTiles], data::sprites::user_interface.png.tiles, DataSize/sizeof(u32));

    // And set the SBB to the correct place
    constexpr auto TileId = SE_PALBANK(15) | (uidefs::UserInterfaceTiles - 1);
    memset32(&UI_SCREEN, TileId | (TileId << 16), sizeof(SCREENBLOCK)/sizeof(u32));

    // Transfer the palette
    memcpy32(&UI_PALETTE, data::sprites::user_interface.png.palette, sizeof(PALBANK)/sizeof(u32));

    // Set the score SBB to the correct place
    for (u32 i = 0; i < uidefs::NumScoreTiles; i++)
        UI_SCREEN[uidefs::ScoreTileBase + i] = SE_PALBANK(15) | (uidefs::ScoreTiles + i);

    // Set the two graphic effects
    clearAlpha = 0;
    reloadAlpha = 12;

    REG_BLDY = reloadAlpha;
}

void UserInterface::vblank()
{
    // Set the tiles here
    constexpr auto TileBase = uidefs::PlayerHealthTileBase;
    const Player& player = gameScene().player;

    int i;
    for (i = 0; i < player.getHealth(); i++)
        UI_SCREEN[TileBase+i] = SE_PALBANK(15) | uidefs::UserInterfaceTiles;
    for (; i < player.getMaxHealth(); i++)
        UI_SCREEN[TileBase+i] = SE_PALBANK(15) | (uidefs::UserInterfaceTiles + 1);
    UI_SCREEN[TileBase+i] = SE_PALBANK(15) | (uidefs::UserInterfaceTiles + 2);

    // Reset the alpha blending
    REG_BLDCNT = BLD_TOP(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_BLACK;

    // Set the two graphic effects
    clearAlpha = 0;
    reloadAlpha = 12;
}

void UserInterface::update()
{
    // Add the two effects
    graphics::hblankEffects.add16(UserInterfaceHeight, &clearAlpha, (void*)&REG_BLDY, 1);
    graphics::hblankEffects.add16(SCREEN_HEIGHT - UserInterfaceHeight - msgBox.openState,
        &reloadAlpha, (void*)&REG_BLDY, 1);

    // Clear the score text
    memset32(&UI_TILE_BANK[uidefs::ScoreTiles], 0, uidefs::NumScoreTiles * sizeof(TILE)/sizeof(u32));

    // Create the text
    char str[] = "      ";
    char vals[6];
    auto end = uintDigits(vals, gameScene().projectiles.getTotalNumProjectiles());
    for (int i = 0; i < end-vals; i++) str[5-i] = '0' + vals[i];

    // And write it
    scoreWriter.write(5, 8, str, 8);

    msgBox.update();
}

GameScene& UserInterface::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, userInterface));
#pragma GCC diagnostic pop
}
