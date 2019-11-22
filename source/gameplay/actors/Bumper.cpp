//--------------------------------------------------------------------------------
// Bumper.cpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#include "Bumper.hpp"
#include "graphics.hpp"

#include "gameplay/GameScene.hpp"

static SinglePaletteAllocator palette EWRAM_BSS (animation_png_palette);

static FrameStore<animation_png_animation::AllocationBlocks, SpriteSize::s16x16_4bpp>
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

Bumper::Bumper(s32f8 x, s32f8 y)
    : animator(frameStore, animation_png_animation::FrameStep + 2 * getRandom(this) - 8),
    palettePtr(palette, false)
{
    pos = vec2(x, y);
    vel = vec2(-1, 1);

    animator.setAnimationPose(animation_png_animation::Animation_Default);
}

void Bumper::update(GameScene& scene)
{
    // Bounce into the map depending on the result
    auto res = scene.map.movementSimulation(pos, vel, BumperWidth, BumperHeight);
    pos += vel + res;
    if (res.x != 0) vel.x = -vel.x;
    if (res.y != 0) vel.y = -vel.y;

    // Update the animator
    animator.update();

    pushGraphics(scene.camera);
}

bool Bumper::updateVisibility(bool visible)
{
    // Set the visibilities
    palettePtr.setActive(visible);
    animator.setVisible(visible);
    return visible;
}

void Bumper::pushGraphics(vec2<s16> camera)
{
    auto dp = vec2<s16>(pos) - camera;

    // Set the animator's visibility
    bool visible = updateVisibility(dp.x > -16 && dp.x < 240 && dp.y > -16 && dp.y < 160);

    // Push the sprite, but only if it's not offscreen
    if (visible) graphics::oam.pushRegular(dp, SpriteSize::s16x16_4bpp, animator.getTileId(), palettePtr.getPalette(), 0);
}