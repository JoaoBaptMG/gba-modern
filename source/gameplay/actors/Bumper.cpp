//--------------------------------------------------------------------------------
// Bumper.cpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#include "Bumper.hpp"
#include "graphics.hpp"

#include "gameplay/GameScene.hpp"
#include "gameplay/collision.hpp"

using namespace data::sprites;

static SinglePaletteAllocator palette EWRAM_BSS (animation.png.palette);

static FrameStore<data::sprites::animation, SpriteSize::s16x16_4bpp> frameStore EWRAM_BSS;

constexpr s32f8 BumperRadius = 8;
constexpr vec2 BumperSize(BumperRadius, BumperRadius);
constexpr auto BumperDeathCounter = 64;

// Quick & dirty way to get a random number (warning: UB)
u16 getRandom(void* thisptr)
{
    auto num = (u32)thisptr >> 4;
    num ^= 0x873a73c3;
    return num & 7;
}

Bumper::Bumper(s32f8 x, s32f8 y, bool dir)
    : animator(frameStore, 30 + 2 * getRandom(this) - 8),
    palettePtr(palette, false)
{
    pos = vec2(x, y);
    size = BumperSize;
    vel = vec2(-1, 1);
    if (dir) vel = -vel;

    animator.setAnimationPose(animation.png.poses.Default);
}

void Bumper::update(GameScene& scene)
{
    // Bounce into the map depending on the result
    auto res = scene.map.movementSimulation(pos, vel, 2*BumperRadius, 2*BumperRadius);
    pos += vel + res;
    if (res.x != 0) vel.x = -vel.x;
    if (res.y != 0) vel.y = -vel.y;

    // If the bumper touches the player, damage the player
    auto& player = scene.player;
    using namespace collision;
    res = circleBox<ResponseType::Normal>(pos + BumperSize, BumperRadius, player.pos, PlayerSize);
    if (res != vec2<s32f8>())
    {
        player.damage();

        // Calculate the reflection vector
        auto n = res.perp();
        vel = -vel + vec2<s32f8>(2 * vel.dot(n) * n);
        pos += vel;
    }

    // Update the animator
    animator.update();

    pushGraphics(scene.camera);
}

void Bumper::respondToMelee(GameScene& scene)
{
    // check the collision and remove the bumper
    auto& player = scene.player;
    if (collision::box<>(pos, BumperSize, player.getMeleePos(), player.getMeleeSize()))
    {
        scene.removeActor(*this);
        scene.addActor<BumperDeath>(pos, animator.getTileId());
    }
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
    auto dp = vec2<int>(pos) - camera;

    // Set the animator's visibility
    bool visible = updateVisibility(dp.x > -16 && dp.x < 240 && dp.y > -16 && dp.y < 160);

    // Push the sprite, but only if it's not offscreen
    if (visible) graphics::oam.pushRegular(dp, SpriteSize::s16x16_4bpp, animator.getTileId(), palettePtr.getPalette(), 0, 4);
}

// Bumper death
BumperDeath::BumperDeath(vec2<s32f8> pos, u16 tileId) : framePointer(frameStore, true), palettePtr(palette, true)
{
    deathCounter = BumperDeathCounter;
    this->pos = pos;
    this->tileId = tileId;
}

void BumperDeath::update(GameScene& scene)
{
    // Delete me when my time is expired
    if (!(deathCounter--)) scene.removeActor(*this);
    pushGraphics(scene.camera);
}

void BumperDeath::pushGraphics(vec2<s16> camera)
{
    // Blink the counter periodically
    if (deathCounter & 2)
    {
        auto dp = vec2<int>(pos) - camera;
        auto offset = BumperDeathCounter - deathCounter;
        auto rdiag = vec2(offset, offset);
        auto ldiag = vec2(-offset, offset);
        constexpr auto Right = vec2((int)BumperRadius, 0);
        constexpr auto Down = vec2(0, (int)BumperRadius);

        graphics::oam.pushRegular(dp - rdiag, SpriteSize::s8x8_4bpp, tileId, palettePtr.getPalette(), 0);
        graphics::oam.pushRegular(dp + Right - ldiag, SpriteSize::s8x8_4bpp, tileId + 1, palettePtr.getPalette(), 0);
        graphics::oam.pushRegular(dp + Down + ldiag, SpriteSize::s8x8_4bpp, tileId + 2, palettePtr.getPalette(), 0);
        graphics::oam.pushRegular(dp + Right + Down + rdiag, SpriteSize::s8x8_4bpp, tileId + 3, palettePtr.getPalette(), 0);
    }
}

