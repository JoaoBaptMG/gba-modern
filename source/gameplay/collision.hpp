//--------------------------------------------------------------------------------
// collision.hpp
//--------------------------------------------------------------------------------
// Provide collision detection routines and utilities
//--------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include "../math/stdfixed.hpp"
#include "../math/vec2.hpp"
#include "../data/BitmaskData.hpp"

enum class CollisionShape : u8 { Circle, Box, Bitmask };

extern "C"
{
    bool circleCircleCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, s16f7 r2) IWRAM_CODE;

    bool boxBoxCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, vec2<s16f7> s2) IWRAM_CODE;

    bool circleBoxCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> s2) IWRAM_CODE;
    bool boxCircleCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, s16f7 r2) IWRAM_CODE;

    bool bitmaskBitmaskCollision(vec2<s16f7> pos1, const BitmaskData* b1, vec2<s16f7> pos2, const BitmaskData* b2) IWRAM_CODE;

    bool circleBitmaskCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, const BitmaskData* b2) IWRAM_CODE;
    bool bitmaskCircleCollision(vec2<s16f7> pos1, const BitmaskData* b1, vec2<s16f7> pos2, s16f7 r2) IWRAM_CODE;
}
