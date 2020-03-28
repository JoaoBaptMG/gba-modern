//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <tonc.h>
#include "graphics/graphics.hpp"
#include "colors.hpp"

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    pal_bg_mem[0] = colors::DarkCyan;

    // Initialize the player
    player.init(PlayerWidth + 8, SCREEN_HEIGHT/2);

    // Initialize the projectile container
    playerProjectiles.init();

    // Initialize the hud
    hud.init();
}

void GameScene::vblank()
{
    hud.vblank();
}

void GameScene::update()
{
    // Update everything
    player.update();
    playerProjectiles.update();
    hud.update();

    // Push the graphics
    player.pushGraphics();
    playerProjectiles.pushGraphics();
}