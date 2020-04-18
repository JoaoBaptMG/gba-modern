//--------------------------------------------------------------------------------
// wavingEnemy.cpp
//--------------------------------------------------------------------------------
// Script for the waving enemies at level 1
//--------------------------------------------------------------------------------
#include "wavingEnemy.hpp"

#include "data/sprites/waving-enemy.hpp"
#include "graphics/StillImagePointer.hpp"
#include "graphics/PalettePointer.hpp"

#include "math/fixedmath.hpp"
#include "math/gcem-degrees.hpp"
#include "gameplay/GameScene.hpp"
#include "util/generateTable.hpp"

// Declaration
static StillImageAllocator image EWRAM_BSS(data::sprites::waving_enemy.png.tiles, SpriteSize::s16x16_4bpp);
static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::waving_enemy.png.palette);

constexpr u32 WavingPeriod = 112;
constexpr auto WavingAmplitude = 48;
constexpr s32f16 Speed = 1.5;

void wavingEnemy(Enemy& enemy, GameScene& gameScene)
{
    enemy.radius = 16;
    enemy.sprSize = SpriteSize::s16x16_4bpp;
    enemy.imagePtr = StillImagePointer(image);
    enemy.palPtr = SinglePalettePointer(palette);
    enemy.health = 5;

    enemy.pos.x = SCREEN_WIDTH+8;
    enemy.pos.y = SCREEN_HEIGHT/2;

    enemy.movementFunction = [i = u32()](Enemy& enemy) mutable
    {
        static const auto MoveYs = generateTable<WavingPeriod>([](u32 i)
        { return u8(SCREEN_HEIGHT/2 - WavingAmplitude * gcem_d::sin(360.0 * i / WavingPeriod)); });

        enemy.pos.x -= Speed;
        enemy.pos.y = MoveYs[i];
        if (++i == WavingPeriod) i = 0;
    };
}