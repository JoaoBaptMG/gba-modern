//--------------------------------------------------------------------------------
// IActor.hpp
//--------------------------------------------------------------------------------
// Provides a base class for the actors of the game
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class GameScene;

class IActor
{
public:
    IActor() {}
    virtual void update(GameScene& scene) {}
    virtual ~IActor() {}

    vec2<s32f8> pos, size;
};

