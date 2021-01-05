//--------------------------------------------------------------------------------
// bigLurker.cpp
//--------------------------------------------------------------------------------
// Script for the big lurker at level 1
//--------------------------------------------------------------------------------
#include "bigLurker.hpp"

#include "data/sprites/big-lurker.hpp"
#include "graphics/StillImagePointer.hpp"
#include "graphics/PalettePointer.hpp"

#include "gameplay/GameScene.hpp"
#include "gameplay/vecUtils.hpp"
#include "util/generateTable.hpp"

constexpr s32f16 MoveSpeed = 0.5;
constexpr double ProjectileSpeed = 0.3;

// Declaration
static StillImageAllocator image EWRAM_BSS(data::sprites::big_lurker.png.tiles, SpriteSize::s32x32_4bpp);
static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::big_lurker.png.palette);

void bigLurker(Enemy& enemy, GameScene& gameScene, s32f16 y)
{
    enemy.pos = vec2<s32f16>(SCREEN_WIDTH + 16, y);
    enemy.movementFunction = [](Enemy& enemy) { enemy.pos.x -= MoveSpeed; };
    enemy.shape = CollisionShape::Bitmask;
    enemy.bitmask = &data::sprites::big_lurker.png.bitmask;
    enemy.sprSize = SpriteSize::s32x32_4bpp;
    enemy.imagePtr = StillImagePointer(image);
    enemy.palPtr = SinglePalettePointer(palette);
    enemy.health = 32;

    constexpr auto NumProjectiles = 24;

    HANDLE_TERM(enemy.waitForFrames(60));
    while (enemy.onScreen())
    {
        static const auto Directions = generateTable<NumProjectiles>([](u32 i)
        { 
            double deg = 360.0 * i / NumProjectiles; 
            return vec2<s16f7>(gcem_d::cos(deg) * ProjectileSpeed, gcem_d::sin(deg) * ProjectileSpeed);
        });

        auto pos = vec2<s16f7>(enemy.pos);
        auto evel = vec2<s16f7>(MoveSpeed, 0);

        for (const auto& vel : Directions)
            gameScene.projectiles.addEnemyProjectile(vec2<s16f7>(pos), vel - evel, 2);

        HANDLE_TERM(enemy.waitForFrames(30));
    }
}