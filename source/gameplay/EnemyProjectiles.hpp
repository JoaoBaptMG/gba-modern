//--------------------------------------------------------------------------------
// EnemyProjectiles.hpp
//--------------------------------------------------------------------------------
// The class that manages the projectiles fired by the enemies
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

#include "Projectile.hpp"
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"

class GameScene;

class EnemyProjectiles final
{
    constexpr static u32 MaxProjectiles = 128;

    GameScene& gameScene();

    Projectile projectiles[MaxProjectiles];
    u32 numProjectiles;

    ObjectTilePointer tilePtr;
    SinglePalettePointer palPtr;

public:
    void init();
    void update();
    void pushGraphics();

    void add(vec2<s16f7> pos, vec2<s16f7> vel, vec2<s16f7> size, u16 tileId)
    {
        ASSERT(numProjectiles < MaxProjectiles);
        projectiles[numProjectiles++] = { pos, vel, size, tileId };
    }
};

