//--------------------------------------------------------------------------------
// collision.hpp
//--------------------------------------------------------------------------------
// Provide collision detection routines and utilities
//--------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

enum class CollisionShape : u8 { Circle, Box, Polygon };

struct PolygonData
{
    vec2<s16f7> halfSize;
    u32 numPoints;
    const vec2<s16f7>* points;
};

bool circleCircleCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, s16f7 r2);

bool boxBoxCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, vec2<s16f7> s2);

bool circleBoxCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> s2);
bool boxCircleCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, s16f7 r2);

bool circlePolygonCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, PolygonData* p2);
bool polygonCircleCollision(vec2<s16f7> pos1, PolygonData* p1, vec2<s16f7> pos2, s16f7 r2);

bool boxPolygonCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, PolygonData* p2);
bool polygonBoxCollision(vec2<s16f7> pos1, PolygonData* p1, vec2<s16f7> pos2, vec2<s16f7> s2);

template <u32 NumPoints>
constexpr PolygonData buildPolygonData(const vec2<s16f7> points[NumPoints])
{
    PolygonData data;
    data.numPoints = NumPoints;
    data.points = points;

    data.halfSize = vec2(abs(points[0].x), abs(points[0].y));
    for (u32 i = 1; i < NumPoints; i++)
    {
        data.halfSize.x = std::max(data.halfSize.x, abs(points[0].x));
        data.halfSize.y = std::max(data.halfSize.y, abs(points[0].y));
    }

    return data;
}
