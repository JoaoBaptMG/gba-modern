//--------------------------------------------------------------------------------
// BitmaskTestScene.cpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#include "BitmaskTestScene.hpp"

#include "data/sprites/player-projectiles.hpp"
#include "data/sprites/bitmask-test.hpp"

#include "gameplay/collision.hpp"
#include "text/mGBADebugging.hpp"
#include "util/profile.hpp"

BitmaskTestScene::BitmaskTestScene() : pos(80, 80)
{
    // Set the display
    REG_DISPCNT = DCNT_MODE3 | DCNT_OBJ | DCNT_OBJ_1D;

    // Load up the sprites
    memcpy32(&tile_mem_obj[1][1], data::sprites::bitmask_test.png.tiles, 16 * sizeof(TILE)/sizeof(u32));

    tile_mem_obj[1][0].data[3] = 0x00011000;
    tile_mem_obj[1][0].data[4] = 0x00011000;

    // Load up the palettes
    memcpy32(&pal_obj_bank[0], data::sprites::player_projectiles.png.palette, sizeof(PALBANK)/sizeof(u32));
    memcpy32(&pal_obj_bank[1], data::sprites::bitmask_test.png.palette, sizeof(PALBANK)/sizeof(u32));
    pal_gradient_ex(&pal_obj_bank[2][0], 1, 16, CLR_RED, CLR_BLACK);
}

void BitmaskTestScene::update()
{
    pos.x += key_tri_horz() * s16f7(0.5);
    pos.y += key_tri_vert() * s16f7(0.5);

    profile::begin16();
    bool result = circleBitmaskCollision(pos, 1, vec2(120, 80), &data::sprites::bitmask_test.png.bitmask);
    auto val = profile::end16();

    auto palIndex = 1 + result;

    // Push the bitmask test at the center
    oamHandles[0].setRegular(vec2<int>(pos.x - 4, pos.y - 4), SpriteSize::s8x8_4bpp, 512, 0, 1);
    oamHandles[1].setRegular(vec2(120 - 16, 80 - 16), SpriteSize::s32x32_4bpp, 513, palIndex, 1);

    if (mgba::isEnabled())
        mgba::log(mgba::Log::Debug, "Execution time: ", val, " cycles.");
}
