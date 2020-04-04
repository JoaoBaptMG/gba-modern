//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <tonc.h>
#include "graphics/graphics.hpp"
#include "colors.hpp"

#include "data/backgrounds/level1.hpp"

void level1(LevelContext& level);

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;

    pal_bg_mem[0] = colors::DarkCyan;

    // Initialize the background
    background.init();
    background.load(data::backgrounds::level1.png);

    // Initialize the player
    player.init(PlayerWidth + 8, SCREEN_HEIGHT/2);

    // Initialize the projectile container
    playerProjectiles.init();

    // Initialize the hud
    hud.init();

    // Initialize the level runner
    level.playLevel(level1);
}

void GameScene::vblank()
{
    hud.vblank();
    background.vblank();
}

void GameScene::update()
{
    // Update everything
    player.update();
    playerProjectiles.update();
    level.update();
    hud.update();

    // Update the enemies
    for (auto& enemy : enemies)
    {
        enemy.update();
        if (enemy.done())
            enemies.remove(&enemy);
    }

    background.offset.x += s32f8(0.5);

    // Push the graphics
    player.pushGraphics();
    playerProjectiles.pushGraphics();

    for (auto& enemy : enemies)
        enemy.pushGraphics();
}