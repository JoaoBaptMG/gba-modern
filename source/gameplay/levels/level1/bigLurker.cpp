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

constexpr s32f16 MoveSpeed = 0.5;
constexpr s16f7 ProjectileSpeed = 2.5;

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

    HANDLE_TERM(enemy.waitForFrames(45));
    while (enemy.onScreen())
    {
        auto ofs1 = vec2<s16f7>(-2, 5);
        auto ofs2 = vec2<s16f7>(-2, -5);

        for (auto ofs : { ofs1, ofs2 })
        {
            auto diff = vec_utils::normalizeLowp(gameScene.player.pos - enemy.pos - ofs);
            auto vel = diff * ProjectileSpeed;
            auto pos = vec2<s16f7>(enemy.pos) + ofs;
            gameScene.enemyProjectiles.add(vec2<s16f7>(pos), vec2<s16f7>(vel), 2);
        }

        HANDLE_TERM(enemy.waitForFrames(45));
    }
}