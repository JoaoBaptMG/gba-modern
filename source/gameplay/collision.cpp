//--------------------------------------------------------------------------------
// collision.cpp
//--------------------------------------------------------------------------------
// Provide collision detection routines and utilities
//--------------------------------------------------------------------------------
#include "collision.hpp"

// TODO: change all of those to IWRAM

bool circleCircleCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, s16f7 r2)
{
    auto diff = pos1.distsq(pos2);
    auto sumr = r1 + r2;
    return diff <= sumr*sumr;
}

bool boxBoxCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, vec2<s16f7> s2)
{
    auto diff = pos1 - pos2;
    diff.x = abs(diff.x);
    diff.y = abs(diff.y);

    auto sums = s1 + s2;
    return diff.x <= sums.x && diff.y <= sums.y;
}

bool circleBoxCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> s2)
{
    auto diff = pos1 - pos2;
    diff.x = abs(diff.x);
    diff.y = abs(diff.y);

    auto sums = vec2(r1, r1) + s2;
    if (!(diff.x <= sums.x && diff.y <= sums.y)) return false;
    if (diff.x >= s2.x && diff.y >= s2.y)
        return (diff - s2).lensq() <= r1*r1;
    return true;
}

bool boxCircleCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, s16f7 r2)
{
    return circleBoxCollision(pos2, r2, pos1, s1);
}
