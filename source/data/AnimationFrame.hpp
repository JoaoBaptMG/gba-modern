//--------------------------------------------------------------------------------
// AnimationFrame.hpp
//--------------------------------------------------------------------------------
// Provides the animation frame structure used for sprites
//--------------------------------------------------------------------------------
#pragma once

#include <tonc_types.h>

struct AnimationFrame final
{
    u8 dx, dy;
    u16 tileIdAttributes;
};
