//--------------------------------------------------------------------------------
// Bumper.cpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#include "Bumper.hpp"
#include "graphics.hpp"

#include "gameplay/GameScene.hpp"
#include "data/sprites/animation.hpp"

static u32 paletteId EWRAM_BSS;
constexpr s32f8 BumperWidth = 16;
constexpr s32f8 BumperHeight = 16;

// Quick & dirty way to get a random number (warning: UB)
u16 getRandom(void* thisptr)
{
    auto num = (u32)thisptr >> 4;
    num ^= 0x873a73c3;
    return num & 7;
}

void Bumper::loadGraphics()
{
    graphics::allocateObjPalettes(1, &paletteId);
    memcpy32(&pal_obj_mem[16 * paletteId], animation_png_palette, sizeof(animation_png_palette) / sizeof(u32));
}

Bumper::Bumper(s32f8 x, s32f8 y)
    : animator(animation_png_tiles, SpriteSize::_16x16_4bpp, animation_png_animation::FrameStep + 2 * getRandom(this) - 8)
{
    pos.x = x;
    pos.y = y;
    pos.vx = -1;
    pos.vy = 1;

    animator.setAnimationPose(animation_png_animation::Animation_Default);
}

void Bumper::update(GameScene& scene)
{
    // Bounce into the map depending on the result
    auto result = scene.map.movementSimulation(pos, BumperWidth, BumperHeight);
    if (result & (CollisionResult::Left | CollisionResult::Right)) pos.vx = -pos.vx;
    if (result & (CollisionResult::Top | CollisionResult::Bottom)) pos.vy = -pos.vy;

    // Update the animator
    animator.update();

    pushGraphics(scene.cameraX, scene.cameraY);
}

void Bumper::pushGraphics(u16 cameraX, u16 cameraY)
{
    auto dx = s16(pos.x) - cameraX;
    auto dy = s16(pos.y) - cameraY;

    // Set the animator's visibility
    bool visible = dx > -16 && dx < 240 && dy > -16 && dy < 160;
    animator.setVisible(visible);

    // Push the sprite, but only if it's not offscreen
    if (visible)
        graphics::pushOAM(
            ATTR0_Y(dy & 255) | ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE,
            ATTR1_X(dx & 511) | ATTR1_SIZE_16x16,
            ATTR2_ID(animator.getTileId()) | ATTR2_PRIO(0) | ATTR2_PALBANK(paletteId));
}