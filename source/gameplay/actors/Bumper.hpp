//--------------------------------------------------------------------------------
// Bumper.hpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#pragma once

#include "IActor.hpp"
#include "math/stdfixed.hpp"
#include "graphics/FrameAnimator.hpp"
#include "graphics/PalettePointer.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

#include "data/sprites/animation.hpp"

using Animator = FrameAnimator<animation_png_animation::AllocationBlocks, SpriteSize::s16x16_4bpp>;

class Bumper final : public IActor
{
    vec2<s32f8> vel;
    Animator animator;
    SinglePalettePointer palettePtr;

    void pushGraphics(vec2<s16> camera);
    bool updateVisibility(bool visible);

public:
    Bumper(s32f8 x, s32f8 y);
    virtual void update(GameScene& scene) override;
    virtual ~Bumper() {}

    // Actions
    void respondToMelee(GameScene& scene) override;
};

class BumperDeath final : public IActor
{
    Animator::Pointer framePointer;
    SinglePalettePointer palettePtr;
    u16 deathCounter, tileId;

    void pushGraphics(vec2<s16> camera);

public:
    BumperDeath(vec2<s32f8> pos, u16 tileId);
    virtual void update(GameScene& scene) override;
    virtual ~BumperDeath() {}
};
