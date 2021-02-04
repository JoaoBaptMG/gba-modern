//--------------------------------------------------------------------------------
// Level.cpp
//--------------------------------------------------------------------------------
// Provides a class for managing a level script and waiting for commands
//--------------------------------------------------------------------------------
#include "Level.hpp"

#include "GameScene.hpp"
#include "util/gba-assert.hpp"
context_t levelContext(context_t ctx, void* arg);

STACKPTR std::byte levelStack[1024];

struct LevelArgs
{
    Level* level;
    LevelScript scriptToRun;
};

context_t levelContext(context_t ctx, void* arg)
{
    auto args = *reinterpret_cast<LevelArgs*>(arg);
    args.level->curCtx = ctx;
    args.scriptToRun(*args.level);
    return args.level->curCtx;
}

void Level::playLevel(LevelScript level)
{
    // Make sure there's not another context going
    ASSERT(!curCtx);

    // Create an args struct to hold the level and the script
    LevelArgs args{this, level};

    // Create the new context and run it for a first time
    curCtx = context_new(levelStack+sizeof(levelStack), levelContext, &args);
    curCtx = context_switch(curCtx);
}

void Level::update()
{
    // If there's no context active, there's no level either, so just skip it
    if (!curCtx) return;

    // The level should take care of setting the next switch time
    if (nextSwitch == -1) return;

    // If the next switch is attained, switch to the level context
    if (curLevelCounter == nextSwitch)
        curCtx = context_switch(curCtx);
    curLevelCounter++;
}

GameScene& Level::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, level));
#pragma GCC diagnostic pop
}

void LevelContext::levelLengthFrames(u16 length)
{
    level().levelLength = length;
}

void LevelContext::atFrames(u16 time)
{
    level().nextSwitch = time;
    level().curCtx = context_switch(level().curCtx);
}

void LevelContext::afterFrames(u16 offset)
{
    level().nextSwitch = level().curLevelCounter + offset;
    level().curCtx = context_switch(level().curCtx);
}

void LevelContext::atEndOfLevel()
{
    level().nextSwitch = level().levelLength;
    level().curCtx = context_switch(level().curCtx);
}

void LevelContext::addEnemy(EnemyScript script)
{
    auto& scene = level().gameScene();
    scene.enemies.add(script, &scene);
}

void LevelContext::showMessage(const char* msg)
{
    auto& scene = level().gameScene();
    scene.hud.msgBox.displayMessage(msg);
}

void LevelContext::showMessage(const char* msg, int period)
{
    auto& scene = level().gameScene();
    scene.hud.msgBox.displayMessage(msg, period);
}

void LevelContext::closeMessage()
{
    auto& scene = level().gameScene();
    scene.hud.msgBox.close();
}

Level& LevelContext::level()
{
    return *static_cast<Level*>(this);
}