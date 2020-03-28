//--------------------------------------------------------------------------------
// Hud.cpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#include "Hud.hpp"

#include "GameScene.hpp"
#include "data/sprites/hud.hpp"

void Hud::init()
{
    // Initialize BG0
    REG_BG0CNT = BG_CBB(3) | BG_SBB(31) | BG_REG_32x32;

    // Transfer the data to the end of the CBB
    constexpr auto DataSize = sizeof(data::sprites::hud.png.tiles);
    memcpy32(&tile_mem[3][448 - DataSize/sizeof(TILE)], data::sprites::hud.png.tiles, DataSize/sizeof(u32));

    // And set the SBB to the correct place
    constexpr auto TileId = SE_PALBANK(15) | (448 - DataSize/sizeof(TILE) - 1);
    memset32(&se_mem[31], TileId | (TileId << 16), sizeof(SCREENBLOCK)/sizeof(u32));

    // Transfer the palette
    memcpy32(&pal_bg_bank[15], data::sprites::hud.png.palette, sizeof(PALBANK)/sizeof(u32));

    // Set the two graphic effects
    clearAlpha = 0;
    reloadAlpha = 8;
}

void Hud::vblank()
{
    // Set the tiles here
    constexpr auto TileBase = 32*1 + 1;
    const Player& player = gameScene().player;

    int i;
    for (i = 0; i < player.getHealth(); i++)
        se_mem[31][TileBase+i] = SE_PALBANK(15) | 445;
    for (; i < player.getMaxHealth(); i++)
        se_mem[31][TileBase+i] = SE_PALBANK(15) | 446;
    se_mem[31][TileBase+i] = 447;

    // Reset the alpha blending
    REG_BLDCNT = BLD_TOP(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_BLACK;
}

void Hud::update()
{
    // Add the two effects
    graphics::hdma.addHdma16(HudSize, &clearAlpha, (void*)&REG_BLDY, 1);
    graphics::hdma.addHdma16(SCREEN_HEIGHT - HudSize, &reloadAlpha, (void*)&REG_BLDY, 1);
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
