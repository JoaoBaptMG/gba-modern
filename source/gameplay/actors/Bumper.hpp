//--------------------------------------------------------------------------------
// Bumper.hpp
//--------------------------------------------------------------------------------
// Actor that bumps through walls
//--------------------------------------------------------------------------------
#pragma once

#include "IActor.hpp"
#include "math/stdfixed.hpp"
#include "gameplay/Position.hpp"
#include "graphics/StreamAnimator.hpp"

class Bumper final : public IActor
{
    Position pos;
    StreamAnimator animator;

    void pushGraphics(u16 cameraX, u16 cameraY);

public:
    Bumper(s32f8 x, s32f8 y);
    virtual void update(GameScene& scene) override;
    virtual ~Bumper() {}

    static void loadGraphics();
};
