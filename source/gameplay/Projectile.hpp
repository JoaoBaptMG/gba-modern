//--------------------------------------------------------------------------------
// Projectile.hpp
//--------------------------------------------------------------------------------
// Base struct for projectile management
//--------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"
#include "collision.hpp"
#include "graphics/SpriteSize.hpp"

struct Projectile
{
    vec2<s16f7> pos, vel;
    u16 type, arg;                  // So it fits nicely in 3 registers
};

struct ProjectileType
{
    CollisionShape shape;
    alignas(void*) vec2<s16f7> halfSize;

    struct SpriteAttrs
    {
        u16 attr0, attr1, attr2, prio;
        alignas(void*) vec2<s16f7> halfSize;
    } attrs;
};

inline constexpr ProjectileType::SpriteAttrs buildSpriteAttrsFor(u16 tileId, SpriteSize sprSize, u16 prio)
{
    auto size = SizeUtils::pixelSize(sprSize);
    return
    { 
        SizeUtils::attr0param(sprSize),
        SizeUtils::attr1param(sprSize),
        u16(ATTR2_PRIO(1) | tileId),
        u16((1 << 14) | (prio << 7)),
        vec2<s16f7>(size/2) };
}

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
    void pushProjectilesToOam(u32 numProjectiles, Projectile* projectiles, OBJ_ATTR* oam,
        const ProjectileType* ptypeArray, u32 attr2prioAdd) IWRAM_CODE;
}
