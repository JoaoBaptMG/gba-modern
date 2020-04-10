//--------------------------------------------------------------------------------
// Enemy.hpp
//--------------------------------------------------------------------------------
// "Holder" class for every enemy in the game
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "math/stdfixed.hpp"
#include "util/context.h"
#include "graphics/SpriteSize.hpp"
#include "graphics/PalettePointer.hpp"
#include "graphics/StillImagePointer.hpp"

class Enemy;
class GameScene;

// The script that controls the enemy
using EnemyScript = void(*)(Enemy&, GameScene&);
enum class ScriptTermination { Terminate, Continue };
#define HANDLE_TERM(expr) do { if ((expr) == ScriptTermination::Terminate) return; } while (false)

class Enemy final
{
public:
    vec2<s32f16> pos, vel, acc, size;    // 32 bytes

private:
    u16 scriptWaitTime;                  // 2 bytes

public:
    SpriteSize sprSize;                  // 2 bytes
    StillImagePointer imagePtr;          // 4 bytes
    SinglePalettePointer palPtr;         // 4 bytes

private:
    context_t curCtx;                    // 4 bytes

public:
    Enemy(EnemyScript script, GameScene* gameScene);
    void update();
    void pushGraphics();
    ~Enemy();

    // Called by script
    [[nodiscard]] ScriptTermination waitForFrames(u16 frames);

    // Utility functions
    [[nodiscard]] inline ScriptTermination moveTo(vec2<s32f16> dest, u16 frames)
    {
        acc = vec2<s32f16>();
        vel = dest / frames;
        if (waitForFrames(frames) == ScriptTermination::Terminate)
            return ScriptTermination::Terminate;
        pos = dest;

        return ScriptTermination::Continue;
    }

    inline bool done() const
    { 
        if (curCtx) return false;
        
        return pos.x < -size.x/2 || pos.x > SCREEN_WIDTH + size.x/2 ||
            pos.y < -size.y/2 || pos.y > SCREEN_HEIGHT + size.y/2;
    }

    friend context_t enemyContext(context_t ctx, void* arg);
};
