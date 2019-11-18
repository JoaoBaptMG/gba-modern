//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <algorithm>
#include "graphics.hpp"

#include "tiled-maps/level1.hpp"

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    // Initialize the map and load a single map
    map.init();
    map.loadMap(map_level1);

    // Reload the graphics data
    graphics::resetObjectsAndPalettes();

    // Initialize the player
    player.init(4 * TileSize, 40 * TileSize);

    cameraX = calculateCameraX();
    cameraY = calculateCameraY();
    // Copy a single screen
    map.copyFullScreen();
    map.loadActors();

    // Initialize the HUD
    hud.init();

    // A dark-ish purple color
    pal_bg_mem[0] = RGB15(14, 0, 14);
}

void GameScene::vblank()
{
    map.vblank();
}

void GameScene::update()
{
    // Do the update
    player.update();

    // Calculate the camera
    cameraX = calculateCameraX();
    cameraY = calculateCameraY();

    hud.pushGraphics();
    map.update();
    player.pushGraphics();

    // Finally update the actors
    for (auto& actor : actors)
        if (actor) actor->update(*this);
}

s16 GameScene::calculateCameraX() const
{
    // The objective is to always let the player in the middle of the camera
    return clamp(s16(player.pos.x) - 120 + PlayerWidth/2, 0, TileSize * map.width() - 240);
}

s16 GameScene::calculateCameraY() const
{
    // Same here
    return clamp(s16(player.pos.y) - 80 + PlayerHeight/2, 0, TileSize * map.height() - 160);
}