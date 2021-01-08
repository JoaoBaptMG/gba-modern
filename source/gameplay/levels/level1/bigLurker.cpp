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

#include "audio/audio.hpp"
#include "data/sounds/enemy-shot1.hpp"

constexpr s32f16 MoveSpeed = 0.5;
constexpr double ProjectileSpeed = 0.9;
constexpr auto NumProjectiles = 16;

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

    HANDLE_TERM(enemy.waitForFrames(60));

    constexpr auto Generator = [](u32 j)
    {
        return generateTable<NumProjectiles>([j](u32 i)
        {
            double deg = 360.0 * (i + 0.5 * j) / NumProjectiles; 
            return vec2<s16f7>(gcem_d::cos(deg) * ProjectileSpeed, gcem_d::sin(deg) * ProjectileSpeed);
        });
    };

    static const std::array Directions{Generator(0), Generator(1)};

    u32 cluster = 0;
    while (enemy.onScreen() && cluster < 14)
    {
        auto pos = vec2<s16f7>(enemy.pos);
        auto evel = vec2<s16f7>(MoveSpeed, 0);

        for (const auto& vel : Directions[cluster&1])
            gameScene.projectiles.addEnemyProjectile(vec2<s16f7>(pos), vel - evel, 2);

        audio::playSound(data::sounds::enemy_shot1.wav, 0.75);
        cluster++;
        HANDLE_TERM(enemy.waitForFrames(20));
    }
}