//--------------------------------------------------------------------------------
// Explosions.hpp
//--------------------------------------------------------------------------------
// Manage the explosions in the game
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"
#include "math/vec2.hpp"

constexpr u32 NumSmallExplosionFrames = 6;
constexpr u32 MaxExplosions = 64;

class Explosions final
{
    struct Explosion
    {
        vec2<s16> pos;
        u16 counter, dummy;
    };

    // Unfortunately, this is necessary (workaround not to make an array allocate a lot)
    struct OamProxy final { UniqueOamHandle obj = UniqueOamHandle::noObj(); };

    Explosion explosions[MaxExplosions];
    OamProxy explosionHandles[MaxExplosions];
    ObjectTilePointer smallExplosion[NumSmallExplosionFrames];
    SinglePalettePointer smallPtr;
    u32 numExplosions;

public:
    void init();
    void update();
    void updateGraphics();

    inline void addSmallExplosion(vec2<s16> pos)
    {
        ASSERT(numExplosions < MaxExplosions);
        explosions[numExplosions] = { pos, 0, 0 };
        explosionHandles[numExplosions].obj = UniqueOamHandle();
    }
};