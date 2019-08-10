//--------------------------------------------------------------------------------
// Position.hpp
//--------------------------------------------------------------------------------
// A class for moveable entities to participate in collision detection
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"

struct Position final
{
    s32f8 x, y, vx, vy;
    void integrate() { x += vx; y += vy; }
};