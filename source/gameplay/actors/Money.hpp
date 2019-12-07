//--------------------------------------------------------------------------------
// Money.hpp
//--------------------------------------------------------------------------------
// A single piece of money
//--------------------------------------------------------------------------------
#pragma once

#include "IActor.hpp"

#include "math/stdfixed.hpp"
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/StillImagePointer.hpp"
#include "graphics/PalettePointer.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class Money final : public IActor
{
    s32f8 yvel, eqy;
    StillImagePointer tilePtr;
    SinglePalettePointer palPtr;

    void pushGraphics(vec2<s16> camera);
    bool updateVisibility(bool visible);

public:
    Money(s32f8 x, s32f8 y);
    virtual void update(GameScene& scene) override;
    virtual ~Money() {}
};
