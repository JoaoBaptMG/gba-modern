//--------------------------------------------------------------------------------
// ProjectileDefinition.hpp
//--------------------------------------------------------------------------------
// This is where all the projectiles are defined, and some other important
// utilities
//--------------------------------------------------------------------------------
#pragma once

#include "collision.hpp"
#include "graphics/SpriteSize.hpp"

struct ProjectileType
{
    CollisionShape shape;
    alignas(void*) vec2<s16f7> halfSize;

    struct SpriteAttrs
    {
        u16 attr0, attr1, attr2, prio;
    } attrs;
};

inline constexpr ProjectileType::SpriteAttrs buildSpriteAttrsFor(u16 tileId, SpriteSize sprSize, u16 prio)
{
    return
    { 
        SizeUtils::attr0param(sprSize),
        SizeUtils::attr1param(sprSize),
        u16(ATTR2_PRIO(1) | tileId),
        prio
    };
}

static const ProjectileType PlayerProjectileTypes[] =
{
    { CollisionShape::Circle, vec2<s16f7>(2, 2), buildSpriteAttrsFor(0, SpriteSize::s8x8_4bpp, 12) }
};

static const ProjectileType EnemyProjectileTypes[] =
{
    { CollisionShape::Circle, vec2<s16f7>(2, 2), buildSpriteAttrsFor(0, SpriteSize::s8x8_4bpp, 0) },
    { CollisionShape::Box, vec2<s16f7>(3, 1), buildSpriteAttrsFor(1, SpriteSize::s8x8_4bpp, 1) },
    { CollisionShape::Circle, vec2<s16f7>(3, 3), buildSpriteAttrsFor(2, SpriteSize::s8x8_4bpp, 2) },
};
