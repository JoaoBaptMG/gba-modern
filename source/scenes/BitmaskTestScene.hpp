//--------------------------------------------------------------------------------
// BitmaskTestScene.hpp
//--------------------------------------------------------------------------------
// A test scene for the bitmask routine
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include <tonc.h>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class BitmaskTestScene final : public IScene
{
    vec2<s16f7> pos;

public:
    BitmaskTestScene();
    virtual void update() override;
};
