//--------------------------------------------------------------------------------
// EnemyProjectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the projectiles fired by the enemies
//--------------------------------------------------------------------------------
#include "EnemyProjectiles.hpp"

#include "GameScene.hpp"
#include "collision.hpp"
#include <algorithm>
#include "util/profile.hpp"
#include "text/mGBADebugging.hpp"

#include "data/sprites/enemy-projectiles.hpp"

constexpr int EnemyProjectilePriority = 5;

static const ProjectileType ProjectileTypes[] =
{
    { CollisionShape::Circle, vec2<s16f7>(2, 2), buildSpriteAttrsFor(0, SpriteSize::s8x8_4bpp, EnemyProjectilePriority) },
    { CollisionShape::Box, vec2<s16f7>(3, 1), buildSpriteAttrsFor(1, SpriteSize::s8x8_4bpp, EnemyProjectilePriority) },
    { CollisionShape::Circle, vec2<s16f7>(3, 3), buildSpriteAttrsFor(2, SpriteSize::s8x8_4bpp, EnemyProjectilePriority) },
};

static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::enemy_projectiles.png.palette);

void EnemyProjectiles::init()
{
    // Set everything to zero
    memset32(projectiles, 0, sizeof(projectiles)/sizeof(u32));
    numProjectiles = 0;

    // Initialize the graphics pointers
    tilePtr = ObjectTilePointer(SpriteSize::s32x8_4bpp);
    tilePtr.setData(data::sprites::enemy_projectiles.png.tiles);

    palPtr = SinglePalettePointer(palette);
}

void EnemyProjectiles::update()
{
    profile::begin32();
    numProjectiles = updateProjectiles(numProjectiles, projectiles);
    auto cyclesUpdate = profile::end32();

    profile::begin32();
    auto ppos = vec2<s16f7>(gameScene().player.pos);
    for (u32 i = 0; i < numProjectiles; i++)
    {
        const auto& ptype = ProjectileTypes[projectiles[i].type];
        using CollisionFunction = bool(*)(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> payload2);
        CollisionFunction collision;

        switch (ptype.shape)
        {
            case CollisionShape::Circle: collision = reinterpret_cast<CollisionFunction>(circleCircleCollision); break;
            case CollisionShape::Box: collision = reinterpret_cast<CollisionFunction>(circleBoxCollision); break;
        }

        if (collision(ppos, PlayerTargetRadius, projectiles[i].pos, ptype.halfSize))
        {
            gameScene().player.damage();
            projectiles[i] = projectiles[--numProjectiles];
            return;
        }
    }
    auto collisionUpdate = profile::end32();

    if (mgba::isEnabled())
    {
        //mgba::log(mgba::Log::Debug, "Enemy projectile update: ", cyclesUpdate, " cycles.");
        //mgba::log(mgba::Log::Debug, "Enemy projectile collision: ", collisionUpdate, " cycles.");
    }
}

void EnemyProjectiles::pushGraphics()
{
    profile::begin32();
    u32 attr2add = tilePtr.getTileId() + ATTR2_PALBANK(palPtr.getPalette()) + (graphics::oam.objCount << 16);
    pushProjectilesToOam(numProjectiles, projectiles, graphics::oam.shadowOAM + graphics::oam.objCount, ProjectileTypes, attr2add);
    graphics::oam.objCount += numProjectiles;
    ASSERT(graphics::oam.objCount <= MaxObjs);

    auto renderingUpdate = profile::end32();
    if (mgba::isEnabled())
    {
        //mgba::log(mgba::Log::Debug, "Enemy projectile rendering: ", renderingUpdate, " cycles.");
        //mgba::log(mgba::Log::Debug, '(', numProjectiles, " enemy projectiles on screen)");
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