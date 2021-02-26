//--------------------------------------------------------------------------------
// wobblingEnemies.hpp
//--------------------------------------------------------------------------------
// Provides a script for the first wobbling enemies on stage 1
//--------------------------------------------------------------------------------
#include "wobblingEnemies.hpp"

#include "data/sprites/wobbling-enemy.hpp"
#include "graphics/StillImagePointer.hpp"
#include "graphics/PalettePointer.hpp"

#include "gameplay/vecUtils.hpp"
#include "scenes/GameScene.hpp"

#include "audio/audio.hpp"
#include "data/sounds/enemy-shot1.hpp"

// Declaration
static StillImageAllocator image EWRAM_BSS(data::sprites::wobbling_enemy.png.tiles, SpriteSize::s16x16_4bpp);
static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::wobbling_enemy.png.palette);

constexpr s32f16 MoveSpeed = 1.5;
constexpr s32f16 Acceleration = 1.0 / 128;

void wobblingEnemyCommon(Enemy& enemy)
{
    enemy.movementFunction = [](Enemy& enemy) { enemy.pos.x -= MoveSpeed; };
    enemy.shape = CollisionShape::Circle;
    enemy.radius = 8;
    enemy.sprSize = SpriteSize::s16x16_4bpp;
    enemy.animationHolder.assign<StillImageHandle>(image);
    enemy.palPtr = SinglePalettePointer(palette);
    enemy.health = 3;
}

void shootTowardsPlayer(Enemy& enemy, GameScene& gameScene);

void wobblingEnemyUp(Enemy& enemy, GameScene& gameScene)
{
    wobblingEnemyCommon(enemy);

    enemy.pos = vec2<s32f16>(SCREEN_WIDTH + 16, 44);
    HANDLE_TERM(enemy.waitForFrames(40));
    shootTowardsPlayer(enemy, gameScene);

    enemy.movementFunction = [vy = s32f16()](Enemy& enemy) mutable
    { 
        vy -= Acceleration;
        enemy.pos += vec2(-MoveSpeed, vy);
    };
}

void wobblingEnemyDown(Enemy& enemy, GameScene& gameScene)
{
    wobblingEnemyCommon(enemy);

    enemy.pos = vec2<s32f16>(SCREEN_WIDTH + 16, SCREEN_HEIGHT - 44);
    HANDLE_TERM(enemy.waitForFrames(40));
    shootTowardsPlayer(enemy, gameScene);

    enemy.movementFunction = [vy = s32f16()](Enemy& enemy) mutable
    { 
        vy += Acceleration;
        enemy.pos += vec2(-MoveSpeed, vy);
    };
}

constexpr s16f7 WobblingProjectileSpeed = 3;

void shootTowardsPlayer(Enemy& enemy, GameScene& gameScene)
{
    // get the difference and normalize it
    auto diff = vec_utils::normalizeLowp(gameScene.player.pos - enemy.pos);
    auto diffp = ROTATE_VEC2(diff, 15);
    auto diffm = ROTATE_VEC2(diff, -15);
    
    for (auto vec : { diff, diffp, diffm })
    {
        auto vel = vec * WobblingProjectileSpeed;
        gameScene.projectiles.addEnemyProjectile(vec2<s16f7>(enemy.pos), vec2<s16f7>(vel), 0);
    }

    audio::playSound(data::sounds::enemy_shot1.wav, audio::Volume(0.5));
}
