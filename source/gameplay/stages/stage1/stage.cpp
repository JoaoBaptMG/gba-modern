//--------------------------------------------------------------------------------
// stage.cpp
//--------------------------------------------------------------------------------
// A stage script
//--------------------------------------------------------------------------------
#include "gameplay/Stage.hpp"
#include "scenes/GameScene.hpp"

#include "wobblingEnemies.hpp"
#include "wavingEnemy.hpp"
#include "bigLurker.hpp"

void stage1(StageContext& stage)
{
    stage.stageLength(30);
    stage.at(0.5);
    stage.showMessage("Use the \2Control Pad\1 to move your ship. Press \2A\1 to shoot, and watch out for the enemies!");

    stage.at(6);
    stage.closeMessage();
    for (int i = 0; i < 7; i++)
    {
        stage.addEnemy(wobblingEnemyUp);
        stage.addEnemy(wobblingEnemyDown);
        stage.after(1.0 / 3.0);
    }

    stage.at(10);
    for (u32 i = 0; i < 12; i++)
    {
        stage.addEnemy([fts = (12 + 25*i) & 127](Enemy& enemy, GameScene& gameScene)
            { wavingEnemy(enemy, gameScene, fts); });
        stage.after(0.3);
    }

    stage.at(20);
    stage.addEnemy([](Enemy& enemy, GameScene& gameScene) { bigLurker(enemy, gameScene, 40); });

    stage.after(2);
    stage.addEnemy([](Enemy& enemy, GameScene& gameScene) { bigLurker(enemy, gameScene, 120); });

    stage.atEndOfStage();
}