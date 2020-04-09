//--------------------------------------------------------------------------------
// level.cpp
//--------------------------------------------------------------------------------
// A level script
//--------------------------------------------------------------------------------
#include "gameplay/Level.hpp"
#include "gameplay/GameScene.hpp"

#include "wobblingEnemies.hpp"

void level1(LevelContext& level)
{
    level.levelLength(30);

    level.at(4);
    for (int i = 0; i < 7; i++)
    {
        level.gameScene().addEnemy(wobblingEnemyUp);
        level.gameScene().addEnemy(wobblingEnemyDown);
        level.after(0.5);
    }

    level.atEndOfLevel();
}