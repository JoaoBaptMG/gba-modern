//--------------------------------------------------------------------------------
// Projectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the player and enemy projectiles
//--------------------------------------------------------------------------------
#include "Projectiles.hpp"

#include "scenes/GameScene.hpp"
#include <algorithm>
#include "collision.hpp"

#include "text/mGBADebugging.hpp"
#include "util/profile.hpp"

#include "data/sprites/player-projectiles.hpp"
#include "data/sprites/enemy-projectiles.hpp"

#include "ProjectileDefinition.hpp"

constexpr u16 NoProjectile = -1;

static SinglePaletteAllocator playerPalette EWRAM_BSS(data::sprites::player_projectiles.png.palette);
static SinglePaletteAllocator enemyPalette EWRAM_BSS(data::sprites::enemy_projectiles.png.palette);

OBJ_ATTR Projectiles::projectileOAM[MaxPlayerProjectiles + MaxEnemyProjectiles];

void Projectiles::init()
{
    // Set everything to zero
    memset32(playerProjectiles, 0, sizeof(playerProjectiles)/sizeof(u32));
    memset32(enemyProjectiles, 0, sizeof(enemyProjectiles)/sizeof(u32));
    numEnemyProjectiles = 0;
    numPlayerProjectiles = 0;

    // Initialize the graphics pointers
    playerTilePtr = ObjectTilePointer(SpriteSize::s8x8_4bpp);
    playerTilePtr.setData(data::sprites::player_projectiles.png.tiles);
    enemyTilePtr = ObjectTilePointer(SpriteSize::s32x8_4bpp);
    enemyTilePtr.setData(data::sprites::enemy_projectiles.png.tiles);

    playerPalPtr = SinglePalettePointer(playerPalette);
    enemyPalPtr = SinglePalettePointer(enemyPalette);

    for (u32 i = 0; i < MaxClusters+1; i++)
        clusterPositions[i] = 512;
}

void Projectiles::update()
{
    updatePlayerProjectiles();
    updateEnemyProjectiles();
}

extern "C" u32 updateProjectiles(u32 numProjectiles, Projectile* projectiles) IWRAM_CODE;

void Projectiles::updatePlayerProjectiles()
{
    numPlayerProjectiles = updateProjectiles(numPlayerProjectiles, playerProjectiles);

    for (auto& enemy : gameScene().enemies)
    {
        auto epos = vec2<s16f7>(enemy.pos);

        for (u32 i = 0; i < numPlayerProjectiles; i++)
        {
            if (playerProjectiles[i].type == NoProjectile) continue;

            const auto& ptype = PlayerProjectileTypes[playerProjectiles[i].type];
            using CollisionFunction = bool(*)(vec2<s16f7>, s16f7, vec2<s16f7>, vec2<s16f7>);
            CollisionFunction collision;

            switch (enemy.shape)
            {
                case CollisionShape::Circle: collision = reinterpret_cast<CollisionFunction>(circleCircleCollision); break;
                case CollisionShape::Box: collision = reinterpret_cast<CollisionFunction>(circleBoxCollision); break;
                case CollisionShape::Bitmask: collision = reinterpret_cast<CollisionFunction>(circleBitmaskCollision); break;
            }

            if (collision(playerProjectiles[i].pos, ptype.halfSize.x, epos, enemy.halfSize))
            {
                if (enemy.damage())
                {
                    gameScene().enemies.remove(&enemy);
                    gameScene().explosions.addSmallExplosion(epos);
                }
                playerProjectiles[i].type = NoProjectile;
                break;
            }
        }
    }

    // std::remove_if returns the pointer to the last element after the removal
    // so I just subtract the original pointer from it to get the number of projectiles
    numPlayerProjectiles = std::remove_if(playerProjectiles, playerProjectiles+numPlayerProjectiles,
        [](const Projectile& proj) { return proj.type == NoProjectile; }) - playerProjectiles;
}

void Projectiles::updateEnemyProjectiles()
{
    numEnemyProjectiles = updateProjectiles(numEnemyProjectiles, enemyProjectiles);

    auto ppos = vec2<s16f7>(gameScene().player.pos);
    for (u32 i = 0; i < numEnemyProjectiles; i++)
    {
        const auto& ptype = EnemyProjectileTypes[enemyProjectiles[i].type];
        using CollisionFunction = bool(*)(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> payload2);
        CollisionFunction collision;

        switch (ptype.shape)
        {
            case CollisionShape::Circle: collision = reinterpret_cast<CollisionFunction>(circleCircleCollision); break;
            case CollisionShape::Box: collision = reinterpret_cast<CollisionFunction>(circleBoxCollision); break;
            case CollisionShape::Bitmask: break;
        }

        if (collision(ppos, PlayerTargetRadius, enemyProjectiles[i].pos, ptype.halfSize))
        {
            gameScene().player.damage();
            enemyProjectiles[i] = enemyProjectiles[--numEnemyProjectiles];
            return;
        }
    }
}

constexpr auto OfsY = 16;
static int frame = 0;
void Projectiles::updateGraphics()
{
    // (the function is separated because we don't need the clusters later, so we can spare some stack)
    profile::begin16();
    computeClusters(clusterPositions);
    fillOAM(clusterPositions);
    auto cycles = profile::end16();

    if (frame++ == 5)
    {
        mgba::log(mgba::Log::Info, "(", projectileClusters[4], ",", cycles, ")");
        frame = 0;
    }
}

void Projectiles::vblank()
{
    // Copy the projectiles to OAM
    u32 preProjPos = graphics::oam.getPreProjPos();
    u32 maxProjectiles = MaxObjs - graphics::oam.getObjCount();

    oam_copy(oam_mem + preProjPos, projectileOAM, projectileClusters[1]);
    if (projectileClusters[1] < maxProjectiles)
        obj_hide_multi(oam_mem + preProjPos + projectileClusters[1], maxProjectiles - projectileClusters[1]);

    // Get the other clusters
    for (u32 i = 0; clusterPositions[i] != 512; i++)
    {
        int line = clusterPositions[i] - OfsY;
        if (line < 0) break;

        u32 begin = projectileClusters[i+1];
        u32 numObjects = projectileClusters[i+2] - begin;
        graphics::hblankEffects.add32(line, projectileOAM + begin, 
            oam_mem + preProjPos, numObjects * sizeof(OBJ_ATTR) / sizeof(u32));
    }
}

GameScene& Projectiles::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, projectiles));
#pragma GCC diagnostic pop
}