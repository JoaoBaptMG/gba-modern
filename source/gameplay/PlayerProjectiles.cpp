//--------------------------------------------------------------------------------
// PlayerProjectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the player projectiles
//--------------------------------------------------------------------------------
#include "PlayerProjectiles.hpp"

#include "GameScene.hpp"
#include <algorithm>
#include "collision.hpp"

#include "text/mGBADebugging.hpp"

#include "data/sprites/player-projectiles.hpp"

constexpr u16 NoProjectile = -1;
constexpr int PlayerProjectilePriority = 7;

static const ProjectileType ProjectileTypes[] =
{
    { CollisionShape::Circle, vec2<s16f7>(2, 2), buildSpriteAttrsFor(0, SpriteSize::s8x8_4bpp, PlayerProjectilePriority) }
};

static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::player_projectiles.png.palette);

void PlayerProjectiles::init()
{
    // Set everything to zero
    memset32(projectiles, 0, sizeof(projectiles)/sizeof(u32));
    numProjectiles = 0;

    // Initialize the graphics pointers
    tilePtr = ObjectTilePointer(SpriteSize::s8x8_4bpp);
    tilePtr.setData(data::sprites::player_projectiles.png.tiles);

    palPtr = SinglePalettePointer(palette);
}

void PlayerProjectiles::update()
{
    setProjectileSortMode(SortMode::Descending);
    numProjectiles = updateProjectiles(numProjectiles, projectiles);
    sortProjectiles(numProjectiles, projectiles);

    for (auto& enemy : gameScene().enemies)
    {
        auto epos = vec2<s16f7>(enemy.pos);

        for (u32 i = 0; i < numProjectiles; i++)
        {
            if (projectiles[i].type == NoProjectile) continue;

            const auto& ptype = ProjectileTypes[projectiles[i].type];
            using CollisionFunction = bool(*)(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, const void* payload2);
            CollisionFunction collision;

            switch (enemy.shape)
            {
                case CollisionShape::Circle: collision = reinterpret_cast<CollisionFunction>(circleCircleCollision); break;
                case CollisionShape::Box: collision = reinterpret_cast<CollisionFunction>(circleBoxCollision); break;
                case CollisionShape::Polygon: break;
            }

            if (collision(projectiles[i].pos, ptype.halfSize.x, epos, enemy.polygonData))
            {
                if (enemy.damage())
                {
                    gameScene().explosions.addSmallExplosion(epos);
                    gameScene().enemies.remove(&enemy);
                }
                projectiles[i].type = NoProjectile;
                break;
            }
        }
    }

    // std::remove_if returns the pointer to the last element after the removal
    // so I just subtract the original pointer from it to get the number of projectiles
    numProjectiles = std::remove_if(projectiles, projectiles+numProjectiles,
        [](const Projectile& proj) { return proj.type == NoProjectile; }) - projectiles;
}

void PlayerProjectiles::pushGraphics()
{
    u32 attr2add = tilePtr.getTileId() + ATTR2_PALBANK(palPtr.getPalette()) + (graphics::oam.objCount << 16);
    pushProjectilesToOam(numProjectiles, projectiles, graphics::oam.shadowOAM + graphics::oam.objCount, ProjectileTypes, attr2add);
    graphics::oam.objCount += numProjectiles;
    ASSERT(graphics::oam.objCount <= MaxObjs);
}

GameScene& PlayerProjectiles::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, playerProjectiles));
#pragma GCC diagnostic pop
}