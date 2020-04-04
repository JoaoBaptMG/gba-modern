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

    level.at(5);
    for (int i = 0; i < 6; i++)
    {
        level.gameScene().addEnemy(wobblingEnemyUp);
        level.gameScene().addEnemy(wobblingEnemyDown);
        level.after(0.8);
    }

    level.atEndOfLevel();
}