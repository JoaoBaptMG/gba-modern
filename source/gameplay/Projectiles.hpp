//--------------------------------------------------------------------------------
// Projectile.hpp
//--------------------------------------------------------------------------------
// The class that manages the player and enemy projectiles
//--------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <tonc.h>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"

struct Projectile
{
    vec2<s16f7> pos, vel;
    u16 type, arg;                  // So it fits nicely in 3 registers
};

class GameScene;

class Projectiles final
{
    constexpr static u32 MaxClusters = 4;
    constexpr static u32 MaxPlayerProjectiles = 64;
    constexpr static u32 MaxEnemyProjectiles = 256;
    static OBJ_ATTR projectileOAM[MaxPlayerProjectiles + MaxEnemyProjectiles];

    GameScene& gameScene();

    void updatePlayerProjectiles();
    void updateEnemyProjectiles();

    void computeClusters(u32* clusterPos);
    void fillOAM(const u32* clusterPos);

    Projectile playerProjectiles[MaxPlayerProjectiles];
    Projectile enemyProjectiles[MaxEnemyProjectiles];
    u32 numPlayerProjectiles, numEnemyProjectiles;

    ObjectTilePointer playerTilePtr, enemyTilePtr;
    SinglePalettePointer playerPalPtr, enemyPalPtr;

    u16 projectileClusters[MaxClusters+1];

public:
    void init();
    void vblank();
    void update();
    void updateGraphics();

    void addPlayerProjectile(vec2<s16f7> pos, vec2<s16f7> vel, u16 type)
    {
        ASSERT(numPlayerProjectiles < MaxPlayerProjectiles);
        playerProjectiles[numPlayerProjectiles++] = { pos, vel, type };
    }

    void addPlayerProjectile(vec2<s16f7> pos, vec2<s16f7> vel, u16 type, u16 arg)
    {
        ASSERT(numPlayerProjectiles < MaxPlayerProjectiles);
        playerProjectiles[numPlayerProjectiles++] = { pos, vel, type, arg };
    }

    void addEnemyProjectile(vec2<s16f7> pos, vec2<s16f7> vel, u16 type)
    {
        ASSERT(numEnemyProjectiles < MaxEnemyProjectiles);
        enemyProjectiles[numEnemyProjectiles++] = { pos, vel, type };
    }

    void addEnemyProjectile(vec2<s16f7> pos, vec2<s16f7> vel, u16 type, u16 arg)
    {
        ASSERT(numEnemyProjectiles < MaxEnemyProjectiles);
        enemyProjectiles[numEnemyProjectiles++] = { pos, vel, type, arg };
    }
};

