//--------------------------------------------------------------------------------
// Projectile.hpp
//--------------------------------------------------------------------------------
// Base struct for projectile management
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

struct Projectile
{
    vec2<s16f7> pos, vel, size;
    u16 tileId;
};

u32 updateProjectiles(Projectile* projectiles, u32 numProjectiles) IWRAM_CODE;
