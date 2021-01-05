//--------------------------------------------------------------------------------
// Enemy.cpp
//--------------------------------------------------------------------------------
// "Holder" class for every enemy in the game
//--------------------------------------------------------------------------------
#include "Enemy.hpp"
#include "util/UnorderedList.hpp"
#include "GameScene.hpp"
#include <array>
#include <cstddef>

// Provide outside storage for the stack
using Stack = std::array<std::byte, 256>;
STACKPTR UnorderedList<Stack, NumEnemies> allStacks EWRAM_BSS;

constexpr auto EnemyPriority = 12;

struct EnemyArgs
{
    Enemy* enemy;
    GameScene* gameScene;
    EnemyScript scriptToRun;
};

context_t enemyContext(context_t ctx, void* arg)
{
    auto args = *reinterpret_cast<EnemyArgs*>(arg);

    args.enemy->curCtx = ctx;
    args.scriptToRun(*args.enemy, *args.gameScene);

    return args.enemy->curCtx;
}

Enemy::Enemy(EnemyScript script, GameScene* gameScene) : pos(), radius()
{
    // Build the args
    EnemyArgs args{this, gameScene, script};

    // Allocate a new stack
    auto& newStack = allStacks.add();

    // Create the new context and run it for a first time
    curCtx = context_new(newStack.data()+newStack.size(), enemyContext, &args);
    curCtx = context_switch(curCtx);
}

void Enemy::update()
{
    // Waits if the script asked to wait, and then switch
    if (curCtx)
    {
        if (scriptWaitTime) scriptWaitTime--;
        else curCtx = context_switch(curCtx);
    }

    movementFunction(*this);

    // Update the invCounter
    if (invCounter > 0) invCounter--;
}

void Enemy::updateGraphics()
{
    // Push the sprite, but only if the invCounter is not active
    auto dp = vec2<int>(pos) - SizeUtils::pixelSize(sprSize)/2;
    if (!invCounter)
        oamHandle.setRegular(dp, sprSize, imagePtr.getTileId(), palPtr.getPalette(), 1, EnemyPriority);
    else oamHandle.setHidden();
}

ScriptTermination Enemy::waitForFrames(u16 frames)
{
    scriptWaitTime = frames;
    curCtx = context_switch(curCtx);
    return scriptWaitTime == (u16)-1 ? ScriptTermination::Terminate
        : ScriptTermination::Continue;
}

bool Enemy::damage(int amount)
{
    if (amount == 0) return false;

    if (health > amount)
    {
        health -= amount;
        invCounter = 2;
        return false;
    }
    else return true;
}

bool Enemy::onScreen() const
{
    vec2<s16f7> comparison{};

    switch (shape)
    {
        case CollisionShape::Circle: comparison = vec2(radius, radius); break;
        case CollisionShape::Box: comparison = halfSize; break;
        case CollisionShape::Bitmask: comparison = bitmask->halfSize; break;
        default: return false;
    }

    return abs(pos.x - SCREEN_WIDTH/2) < SCREEN_WIDTH/2 + comparison.x && 
        abs(pos.y - SCREEN_HEIGHT/2) < SCREEN_HEIGHT/2 + comparison.y;
}

Enemy::~Enemy()
{
    if (curCtx)
    {
        scriptWaitTime = -1;
        curCtx = context_switch(curCtx);
        ASSERT(!curCtx);
    }
}