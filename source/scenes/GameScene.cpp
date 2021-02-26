//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a stage
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <tonc.h>
#include "graphics/graphics.hpp"
#include "colors.hpp"

#include "data/backgrounds/stage1.hpp"

void stage1(StageContext& stage);

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;

    pal_bg_mem[0] = colors::DarkCyan;

    // Initialize the background
    background.init();
    background.load(data::backgrounds::stage1.png);

    // Initialize the player
    player.init(PlayerWidth + 8, SCREEN_HEIGHT/2);

    // Initialize the projectile containers
    graphics::oam.setProjectilePrio(6);
    projectiles.init();

    // Initialize the explosion container
    explosions.init();

    // Initialize the userInterface
    userInterface.init();

    // Initialize the stage runner
    stage.playStage(stage1);
    userInterface.displayStage(1);
}

void GameScene::vblank()
{
    userInterface.vblank();
    projectiles.vblank();
    background.vblank();
}

void GameScene::update()
{
    // Update everything
    player.update();
    stage.update();
    userInterface.update();

    // Update the enemies
    for (auto& enemy : enemies)
    {
        enemy.update();
        if (enemy.done())
            enemies.remove(&enemy);
    }

    projectiles.update();
    explosions.update();

    background.offset.x += s32f8(0.5);

    // Push the graphics
    player.updateGraphics();

    for (auto& enemy : enemies)
        enemy.updateGraphics();

    explosions.updateGraphics();
    projectiles.updateGraphics();
}