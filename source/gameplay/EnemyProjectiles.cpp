//--------------------------------------------------------------------------------
// EnemyProjectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the projectiles fired by the enemies
//--------------------------------------------------------------------------------
#include "EnemyProjectiles.hpp"

#include "GameScene.hpp"
#include <algorithm>

#include "data/sprites/enemy-projectiles.hpp"

constexpr int EnemyProjectilePriority = 5;

static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::enemy_projectiles.png.palette);

void EnemyProjectiles::init()
{
    // Set everything to zero
    memset32(projectiles, 0, sizeof(projectiles)/sizeof(u32));
    numProjectiles = 0;

    // Initialize the graphics pointers
    tilePtr = ObjectTilePointer(SpriteSize::s8x8_4bpp);
    tilePtr.setData(data::sprites::enemy_projectiles.png.tiles);

    palPtr = SinglePalettePointer(palette);
}

void EnemyProjectiles::update()
{
    numProjectiles = updateProjectiles(projectiles, numProjectiles);
}

void EnemyProjectiles::pushGraphics()
{
    // Add a sprite for each projectile on screen
    for (u32 i = 0; i < numProjectiles; i++)
    {
        auto dp = vec2<int>(projectiles[i].pos) - vec2(4, 4);
        graphics::oam.pushRegular(dp, SpriteSize::s8x8_4bpp,
            tilePtr.getTileId() + projectiles[i].tileId,
            palPtr.getPalette(), 0, EnemyProjectilePriority);
    }
}

GameScene& EnemyProjectiles::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, enemyProjectiles));
#pragma GCC diagnostic pop
}