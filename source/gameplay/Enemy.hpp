//--------------------------------------------------------------------------------
// Enemy.hpp
//--------------------------------------------------------------------------------
// "Holder" class for every enemy in the game
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "math/stdfixed.hpp"
#include "util/context.h"
#include "util/TrivialStaticFunction.hpp"
#include "graphics/SpriteSize.hpp"
#include "graphics/PalettePointer.hpp"
#include "graphics/AnimationHolder.hpp"

#include "collision.hpp"

class Enemy;
class GameScene;

// The script that controls the enemy
using EnemyScript = TrivialStaticFunction<12, void(Enemy&, GameScene&)>;
using MovementFunction = TrivialStaticFunction<12, void(Enemy&)>;
enum class ScriptTermination { Terminate, Continue };
#define HANDLE_TERM(expr) do { if ((expr) == ScriptTermination::Terminate) return; } while (false)

class Enemy final
{
public:
    vec2<s32f16> pos;                    // 8 bytes
    u16 health;                          // 2 bytes
    SpriteSize sprSize;                  // 1 bytes
    CollisionShape shape;                // 1 byte

    union                                // 4 bytes
    {
        s16f7 radius;
        vec2<s16f7> halfSize;
        const BitmaskData* bitmask;
    };
    
    AnimationHolder animationHolder;     // 32 bytes
    SinglePalettePointer palPtr;         // 4 bytes
    MovementFunction movementFunction;   // 16 bytes

private:
    context_t curCtx;                    // 4 bytes
    u16 scriptWaitTime;                  // 2 bytes
    u8 invCounter;                       // 1 byte
    UniqueOamHandle oamHandle;           // 1 byte

public:
    Enemy(EnemyScript script, GameScene* gameScene);
    void update();
    void updateGraphics();
    ~Enemy();

    // Called by script
    [[nodiscard]] ScriptTermination waitForFrames(u16 frames);

    inline bool done() const
    { 
        if (curCtx) return false;
        return !onScreen();
    }

    bool onScreen() const;

    bool damage(int amount = 1);

    friend context_t enemyContext(context_t ctx, void* arg);
};
