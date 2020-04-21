//--------------------------------------------------------------------------------
// EnemyProjectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the projectiles fired by the enemies
//--------------------------------------------------------------------------------
#include "EnemyProjectiles.hpp"

#include "GameScene.hpp"
#include <algorithm>

#include "data/sprites/enemy-projectiles.hpp"

struct EnemyProjectileType
{
    u16 tileId;
    SpriteSize spriteSize;
    u8 shape;
    vec2<s16f7> halfSize;
};

const EnemyProjectileType ProjectileTypes[] =
{
    { 0, SpriteSize::s8x8_4bpp, 0, vec2<s16f7>(2, 2) }
};

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
    setProjectileSortMode(SortMode::Ascending);
    numProjectiles = updateProjectiles(numProjectiles, projectiles);
    sortProjectiles(numProjectiles, projectiles);

    auto ppos = vec2<s16f7>(gameScene().player.pos);
    for (u32 i = 0; i < numProjectiles; i++)
    {
        auto radius = ProjectileTypes[projectiles[i].type].halfSize.x;
        auto diff = (ppos - projectiles[i].pos).lensq();
        auto sumr = radius + PlayerTargetRadius;

        if (diff < sumr*sumr)
        {
            gameScene().player.damage();
            projectiles[i] = projectiles[--numProjectiles];
            return;
        }
    }
}

void EnemyProjectiles::pushGraphics()
{
    // Add a sprite for each projectile on screen
    for (u32 i = 0; i < numProjectiles; i++)
    {
        const auto& ptype = ProjectileTypes[projectiles[i].type];
        auto dp = vec2<int>(projectiles[i].pos) - SizeUtils::pixelSize(ptype.spriteSize)/2;
        graphics::oam.pushRegular(dp, ptype.spriteSize, tilePtr.getTileId() + ptype.tileId,
            palPtr.getPalette(), 1, EnemyProjectilePriority);
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