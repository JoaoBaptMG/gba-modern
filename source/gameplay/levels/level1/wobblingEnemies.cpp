//--------------------------------------------------------------------------------
// wobblingEnemies.hpp
//--------------------------------------------------------------------------------
// Provides a script for the first wobbling enemies on level 1
//--------------------------------------------------------------------------------
#include "wobblingEnemies.hpp"

#include "data/sprites/wobbling-enemy.hpp"
#include "graphics/StillImagePointer.hpp"
#include "graphics/PalettePointer.hpp"

// Declaration
static StillImageAllocator image EWRAM_BSS(data::sprites::wobbling_enemy.png.tiles, SpriteSize::s16x16_4bpp);
static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::wobbling_enemy.png.palette);

void wobblingEnemyCommon(Enemy& enemy)
{
    enemy.vel.x = -1;
    enemy.size = vec2<s16f7>(16, 16);
    enemy.sprSize = SpriteSize::s16x16_4bpp;
    enemy.imagePtr = StillImagePointer(image);
    enemy.palPtr = SinglePalettePointer(palette);
}

void wobblingEnemyUp(Enemy& enemy)
{
    wobblingEnemyCommon(enemy);

    enemy.pos = vec2<s16f7>(SCREEN_WIDTH + 16, 32);
    HANDLE_TERM(enemy.waitForFrames(90));
    enemy.acc.y = -s16f7::epsilon;
}

void wobblingEnemyDown(Enemy& enemy)
{
    wobblingEnemyCommon(enemy);

    enemy.pos = vec2<s16f7>(SCREEN_WIDTH + 16, SCREEN_HEIGHT - 32);
    HANDLE_TERM(enemy.waitForFrames(90));
    enemy.acc.y = s16f7::epsilon;
}
