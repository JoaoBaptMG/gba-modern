//--------------------------------------------------------------------------------
// Enemy.cpp
//--------------------------------------------------------------------------------
// "Holder" class for every enemy in the game
//--------------------------------------------------------------------------------
#include "Enemy.hpp"

constexpr auto EnemyPriority = 6;

struct EnemyArgs
{
    Enemy* enemy;
    EnemyScript scriptToRun;
};

context_t enemyContext(context_t ctx, void* arg)
{
    auto args = *reinterpret_cast<EnemyArgs*>(arg);

    args.enemy->curCtx = ctx;
    args.scriptToRun(*args.enemy);

    return args.enemy->curCtx;
}

Enemy::Enemy(EnemyScript script) : pos(), vel(), acc(), size()
{
    // Build the args
    EnemyArgs args{this, script};

    // Create the new context and run it for a first time
    curCtx = context_new(ctxStack+sizeof(ctxStack), enemyContext, &args);
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

    // Simple integration
    pos += vel;
    vel += acc;
}

void Enemy::pushGraphics()
{
    // Push the sprite
    auto dp = vec2<int>(pos) - size/2;
    graphics::oam.pushRegular(dp, sprSize, imagePtr.getTileId(), palPtr.getPalette(), 0, EnemyPriority);
}

ScriptTermination Enemy::waitForFrames(u16 frames)
{
    scriptWaitTime = frames;
    curCtx = context_switch(curCtx);
    return scriptWaitTime == -1 ? ScriptTermination::Terminate
        : ScriptTermination::Continue;
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