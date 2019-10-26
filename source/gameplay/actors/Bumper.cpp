//--------------------------------------------------------------------------------
// Bumper.cpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#include "Bumper.hpp"
#include "graphics.hpp"

#include "gameplay/GameScene.hpp"
#include "data/sprites/bumper.hpp"

static u32 tileId EWRAM_BSS, paletteId EWRAM_BSS;
constexpr s32f8 BumperWidth = 16;
constexpr s32f8 BumperHeight = 16;

void Bumper::loadGraphics()
{
    tileId = graphics::allocateObjTiles(SpriteSize::_16x16_4bpp);
    graphics::allocateObjPalettes(1, &paletteId);

    memcpy32(&tile_mem_obj[0][tileId], bumper_png_tiles, sizeof(bumper_png_tiles) / sizeof(u32));
    memcpy32(&pal_obj_mem[16*paletteId], bumper_png_palette, sizeof(bumper_png_palette) / sizeof(u32));
}

Bumper::Bumper(s32f8 x, s32f8 y)
{
    pos.x = x;
    pos.y = y;
    pos.vx = -1;
    pos.vy = 1;
}

void Bumper::update(GameScene& scene)
{
    // Bounce into the map depending on the result
    auto result = scene.map.movementSimulation(pos, BumperWidth, BumperHeight);
    if (result & (CollisionResult::Left | CollisionResult::Right)) pos.vx = -pos.vx;
    if (result & (CollisionResult::Top | CollisionResult::Bottom)) pos.vy = -pos.vy;

    pushGraphics(scene.cameraX, scene.cameraY);
}

void Bumper::pushGraphics(u16 cameraX, u16 cameraY)
{
    auto dx = s16(pos.x) - cameraX;
    auto dy = s16(pos.y) - cameraY;

    // Push the sprite, but only if it's not offscreen
    if (dx > -16 && dx < 240 && dy > -16 && dy < 160)
        graphics::pushOAM(
            ATTR0_Y(dy & 255) | ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE,
            ATTR1_X(dx & 511) | ATTR1_SIZE_16x16,
            ATTR2_ID(tileId) | ATTR2_PRIO(0) | ATTR2_PALBANK(paletteId));
}