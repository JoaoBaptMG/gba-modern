//--------------------------------------------------------------------------------
// GameScene.cpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#include "GameScene.hpp"

#include <algorithm>
#include "graphics/graphics.hpp"

GameScene::GameScene()
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    // A dark-ish purple color
    pal_bg_mem[0] = RGB15(14, 0, 14);
}

void GameScene::vblank()
{
    
}

void GameScene::update()
{

}