//--------------------------------------------------------------------------------
// Stage.cpp
//--------------------------------------------------------------------------------
// Provides a class for managing a stage script and waiting for commands
//--------------------------------------------------------------------------------
#include "Stage.hpp"

#include "GameScene.hpp"
#include "util/gba-assert.hpp"
context_t stageContext(context_t ctx, void* arg);

STACKPTR std::byte stageStack[1024];

struct StageArgs
{
    Stage* stage;
    StageScript scriptToRun;
};

context_t stageContext(context_t ctx, void* arg)
{
    auto args = *reinterpret_cast<StageArgs*>(arg);
    args.stage->curCtx = ctx;
    args.scriptToRun(*args.stage);
    return args.stage->curCtx;
}

void Stage::playStage(StageScript stage)
{
    // Make sure there's not another context going
    ASSERT(!curCtx);

    // Create an args struct to hold the stage and the script
    StageArgs args{this, stage};

    // Create the new context and run it for a first time
    curCtx = context_new(stageStack+sizeof(stageStack), stageContext, &args);
    curCtx = context_switch(curCtx);
}

void Stage::update()
{
    // If there's no context active, there's no stage either, so just skip it
    if (!curCtx) return;

    // The stage should take care of setting the next switch time
    if (nextSwitch == -1) return;

    // If the next switch is attained, switch to the stage context
    if (curStageCounter == nextSwitch)
        curCtx = context_switch(curCtx);
    curStageCounter++;
}

GameScene& Stage::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, stage));
#pragma GCC diagnostic pop
}

void StageContext::stageLengthFrames(u16 length)
{
    stage().stageLength = length;
}

void StageContext::atFrames(u16 time)
{
    stage().nextSwitch = time;
    stage().curCtx = context_switch(stage().curCtx);
}

void StageContext::afterFrames(u16 offset)
{
    stage().nextSwitch = stage().curStageCounter + offset;
    stage().curCtx = context_switch(stage().curCtx);
}

void StageContext::atEndOfStage()
{
    stage().nextSwitch = stage().stageLength;
    stage().curCtx = context_switch(stage().curCtx);
}

void StageContext::addEnemy(EnemyScript script)
{
    auto& scene = stage().gameScene();
    scene.enemies.add(script, &scene);
}

void StageContext::showMessage(const char* msg)
{
    auto& scene = stage().gameScene();
    scene.userInterface.msgBox.displayMessage(msg);
}

void StageContext::showMessage(const char* msg, int period)
{
    auto& scene = stage().gameScene();
    scene.userInterface.msgBox.displayMessage(msg, period);
}

void StageContext::closeMessage()
{
    auto& scene = stage().gameScene();
    scene.userInterface.msgBox.close();
}

Stage& StageContext::stage()
{
    return *static_cast<Stage*>(this);
}