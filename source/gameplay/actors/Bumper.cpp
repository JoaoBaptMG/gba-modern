//--------------------------------------------------------------------------------
// Bumper.cpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#include "Bumper.hpp"
#include "graphics.hpp"

#include "gameplay/GameScene.hpp"

static SinglePaletteAllocator palette EWRAM_BSS (animation_png_palette);

static FrameStore<animation_png_animation::AllocationBlocks, SpriteSize::_16x16_4bpp>
    frameStore EWRAM_BSS (animation_png_tiles);

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
}

Bumper::Bumper(s32f8 x, s32f8 y)
    : animator(frameStore, animation_png_animation::FrameStep + 2 * getRandom(this) - 8),
    palettePtr(palette, false)
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

bool Bumper::updateVisibility(bool visible)
{
    // Set the visibilities
    palettePtr.setActive(visible);
    animator.setVisible(visible);
    return visible;
}

void Bumper::pushGraphics(u16 cameraX, u16 cameraY)
{
    auto dx = s16(pos.x) - cameraX;
    auto dy = s16(pos.y) - cameraY;

    // Set the animator's visibility
    bool visible = updateVisibility(dx > -16 && dx < 240 && dy > -16 && dy < 160);

    // Push the sprite, but only if it's not offscreen
    if (visible)
        graphics::pushOAM(
            ATTR0_Y(dy & 255) | ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE,
            ATTR1_X(dx & 511) | ATTR1_SIZE_16x16,
            ATTR2_ID(animator.getTileId()) | ATTR2_PRIO(0) | ATTR2_PALBANK(palettePtr.getPalette()));
}