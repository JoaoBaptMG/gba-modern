//--------------------------------------------------------------------------------
// Stage.hpp
//--------------------------------------------------------------------------------
// Provides a class for managing a stage script and waiting for commands
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "util/context.h"
#include "Enemy.hpp"

class GameScene;
class Stage;

// The StageContext is a class to avoid that the coroutines call unsafe functions of Stage
class StageContext
{
    Stage& stage();

public:
    void stageLengthFrames(u16 length);
    void atFrames(u16 time);
    void afterFrames(u16 offset);
    void atEndOfStage();
    void addEnemy(EnemyScript script);
    void showMessage(const char* msg);
    void showMessage(const char* msg, int period);
    void closeMessage();

    inline void stageLength(double secs) { stageLengthFrames(60 * secs); }
    inline void at(double secs) { atFrames(60 * secs); }
    inline void after(double secs) { afterFrames(60 * secs); }

    friend class Stage;
};

using StageScript = void(*)(StageContext&);

class Stage final : public StageContext
{
    GameScene& gameScene();
    context_t curCtx;

public:
    Stage() : curCtx(nullptr), curStageCounter(0), nextSwitch(-1), stageLength(0) {}
    void playStage(StageScript stage);
    void update();

    u16 curStageCounter, nextSwitch, stageLength;
    friend class StageContext;
    friend context_t stageContext(context_t ctx, void* arg);
};
