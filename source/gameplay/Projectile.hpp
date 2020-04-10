//--------------------------------------------------------------------------------
// Projectile.hpp
//--------------------------------------------------------------------------------
// Base struct for projectile management
//--------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

struct Projectile
{
    vec2<s16f7> pos, vel, size;
    u16 tileId, dummy;              // So it fits nicely in 4 registers
};

extern "C"
{
    u32 updateProjectiles(u32 numProjectiles, Projectile* projectiles) IWRAM_CODE;
    void sortProjectiles(u32 numProjectiles, Projectile* projectiles) IWRAM_CODE;
}
