//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <algorithm>
#include "graphics/graphics.hpp"

#include "tiled-maps/level1.hpp"

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    // Initialize the map and load a single map
    map.init();
    map.loadMap(tiled_maps::level1);

    // Reload the graphics data
    graphics::resetObjectsAndPalettes();

    // Initialize the player
    player.init(4 * TileSize, 40 * TileSize);

    camera = calculateCameraVector();
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
    camera = calculateCameraVector();

    hud.pushGraphics();
    map.update();
    player.pushGraphics();

    // Finally update the actors
    for (auto& actor : actors) actor.update(*this);

    // And remove them
    for (std::size_t i = 0; i < NumActors; i++)
        if (actorsToRemove.test(i)) actors.removeIndex(i);
    actorsToRemove.resetAll();
}

vec2<s16> GameScene::calculateCameraVector() const
{
    // The objective is to always let the player in the middle of the camera
    return vec2<s16>(clamp(s16(player.pos.x) - SCREEN_WIDTH/2 + PlayerWidth/2, 0, TileSize * map.width() - SCREEN_WIDTH),
        clamp(s16(player.pos.y) - SCREEN_HEIGHT/2 + PlayerHeight/2, 0, TileSize * map.height() - SCREEN_HEIGHT));
}