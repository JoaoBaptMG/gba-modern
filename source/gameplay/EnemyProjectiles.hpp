//--------------------------------------------------------------------------------
// EnemyProjectiles.hpp
//--------------------------------------------------------------------------------
// The class that manages the projectiles fired by the enemies
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

#include "math/stdfixed.hpp"
#include "math/vec2.hpp"
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"

class GameScene;

class EnemyProjectiles final
{
    constexpr static u32 MaxProjectiles = 128;

    GameScene& gameScene();

    struct Projectile
    {
        vec2<s16f7> pos, vel;
    };

    Projectile projectiles[MaxProjectiles];
    u32 numProjectiles;

    ObjectTilePointer tilePtr;
    SinglePalettePointer palPtr;

public:
    void init();
    void update();
    void pushGraphics();

    void add(vec2<s16f7> pos, vec2<s16f7> vel)
    {
        ASSERT(numProjectiles < MaxProjectiles);
        projectiles[numProjectiles++] = { pos, vel };
    }
};

