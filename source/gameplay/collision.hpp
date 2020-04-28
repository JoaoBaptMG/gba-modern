//--------------------------------------------------------------------------------
// collision.hpp
//--------------------------------------------------------------------------------
// Provide collision detection routines and utilities
//--------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

enum class CollisionShape : u8 { Circle, Box, Bitmask };

struct BitmaskData;
// The bitmask data is only used in ARM assembly, so it is not a big deal
// essentially, the first 4 bytes are a vec2<s16f7> representing the bitmask's
// bounding box. Let pixelHeight = 2*floor(bbox.y) and numClusters = ceil(bbox.x/16)
// We will have pixelHeight hwords forming a cluster that represents the bitmask,
// from top to bottom, and there are numClusters clusters from left to right.
// The bitmask is aligned to the top-left of the bounding box

extern "C"
{
    bool circleCircleCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, s16f7 r2) IWRAM_CODE;

    bool boxBoxCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, vec2<s16f7> s2) IWRAM_CODE;

    bool circleBoxCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> s2) IWRAM_CODE;
    bool boxCircleCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, s16f7 r2) IWRAM_CODE;
}
