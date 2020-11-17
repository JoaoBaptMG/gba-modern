//--------------------------------------------------------------------------------
// level.cpp
//--------------------------------------------------------------------------------
// A level script
//--------------------------------------------------------------------------------
#include "gameplay/Level.hpp"
#include "gameplay/GameScene.hpp"

#include "wobblingEnemies.hpp"
#include "wavingEnemy.hpp"
#include "bigLurker.hpp"

void level1(LevelContext& level)
{
    level.levelLength(30);

    level.at(4);
    for (int i = 0; i < 7; i++)
    {
        level.addEnemy(wobblingEnemyUp);
        level.addEnemy(wobblingEnemyDown);
        level.after(1.0 / 3.0);
    }

    level.at(8);
    for (u32 i = 0; i < 12; i++)
    {
        level.addEnemy([fts = (12 + 25*i) & 127](Enemy& enemy, GameScene& gameScene)
            { wavingEnemy(enemy, gameScene, fts); });
        level.after(0.3);
    }

    level.at(16);
    level.addEnemy([](Enemy& enemy, GameScene& gameScene) { bigLurker(enemy, gameScene, 40); });

    level.atEndOfLevel();
}