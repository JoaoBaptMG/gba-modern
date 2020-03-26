//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <algorithm>
#include "graphics/graphics.hpp"
#include "colors.hpp"

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    // A very bright red color
    pal_bg_mem[0] = colors::Red;
}

void GameScene::vblank()
{
    
}

void GameScene::update()
{

}