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
    vec2<s16f7> pos, vel;
    u16 type, arg;                  // So it fits nicely in 3 registers
};

extern "C"
{
    // Those values seem like magic, but setProjectileSortMode will set those to
    // four branch instructions inside IWRAM that control the swapping.
    // 0xCA and 0xBA are the bytes to bgt and blt respectively, and those will
    // controll inside how sortProjectiles works
    enum class SortMode { Ascending = 0xCA, Descending = 0xBA };

    u32 updateProjectiles(u32 numProjectiles, Projectile* projectiles) IWRAM_CODE;
    void sortProjectiles(u32 numProjectiles, Projectile* projectiles) IWRAM_CODE;
    void setProjectileSortMode(SortMode sortMode) IWRAM_CODE;
}
