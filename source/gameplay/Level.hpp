//--------------------------------------------------------------------------------
// Level.hpp
//--------------------------------------------------------------------------------
// Provides a class for managing a level script and waiting for commands
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "util/context.h"
#include "Enemy.hpp"

class GameScene;
class Level;

// The LevelContext is a class to avoid that the coroutines call unsafe functions of Level
class LevelContext
{
    Level& level();

public:
    void levelLengthFrames(u16 length);
    void atFrames(u16 time);
    void afterFrames(u16 offset);
    void atEndOfLevel();
    void addEnemy(EnemyScript script);

    inline void levelLength(double secs) { levelLengthFrames(60 * secs); }
    inline void at(double secs) { atFrames(60 * secs); }
    inline void after(double secs) { afterFrames(60 * secs); }

    friend class Level;
};

using LevelScript = void(*)(LevelContext&);

class Level final : public LevelContext
{
    GameScene& gameScene();
    context_t curCtx;

public:
    Level() : curCtx(nullptr), curLevelCounter(0), nextSwitch(-1), levelLength(0) {}
    void playLevel(LevelScript level);
    void update();

    u16 curLevelCounter, nextSwitch, levelLength;
    friend class LevelContext;
    friend context_t levelContext(context_t ctx, void* arg);
};
