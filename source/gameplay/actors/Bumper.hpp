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

using Animator = FrameAnimator<animation_png_animation::AllocationBlocks, SpriteSize::_16x16_4bpp>;

class Bumper final : public IActor
{
    vec2<s32f8> pos, vel;
    Animator animator;
    SinglePalettePointer palettePtr;

    void pushGraphics(vec2<s16> camera);
    bool updateVisibility(bool visible);

public:
    Bumper(s32f8 x, s32f8 y);
    virtual void update(GameScene& scene) override;
    virtual ~Bumper() {}
};
