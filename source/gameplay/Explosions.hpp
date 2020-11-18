//--------------------------------------------------------------------------------
// Explosions.hpp
//--------------------------------------------------------------------------------
// Manage the explosions in the game
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "../graphics/ObjectTilePointer.hpp"
#include "../graphics/PalettePointer.hpp"
#include "../math/vec2.hpp"

constexpr u32 NumSmallExplosionFrames = 6;
constexpr u32 MaxExplosions = 64;

class Explosions final
{
    struct Explosion
    {
        vec2<s16> pos;
        u16 counter, dummy;
    };

    Explosion explosions[MaxExplosions];
    ObjectTilePointer smallExplosion[NumSmallExplosionFrames];
    SinglePalettePointer smallPtr;
    u32 numExplosions;

public:
    void init();
    void update();
    void pushGraphics();

    inline void addSmallExplosion(vec2<s16> pos)
    {
        ASSERT(numExplosions < MaxExplosions);
        explosions[numExplosions++] = { pos, 0, 0 };
    }
};