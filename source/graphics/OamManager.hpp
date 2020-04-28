//--------------------------------------------------------------------------------
// OamManager.hpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "SpriteSize.hpp"
#include "util/gba-assert.hpp"
#include "math/vec2.hpp"

// The maximum number of objects
constexpr auto MaxObjs = 128;

// Enum important enums
enum class SpriteFlip { None, Horizontal, Vertical, Both };
enum class GfxMode { None, AlphaBlending, Window };

class OamManager final
{
public:
    // The required array and size
    u32 objCount;
    alignas(void*) OBJ_ATTR shadowOAM[MaxObjs];

    OamManager() {}
    void init() { oam_init(shadowOAM, MaxObjs); }
    void copyToOAM();

    void pushAttrs(u16 attr0, u16 attr1, u16 attr2, u16 prio = 0);

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, GfxMode mode, bool mosaic, int objPrio = 0)
    {
        pushAttrs((pos.y & 255) | (int(mode) << 10) | (mosaic ? ATTR0_MOSAIC : 0) | SizeUtils::attr0param(size),
            (pos.x & 511) | (int(flip) << 12) | SizeUtils::attr1param(size),
            ATTR2_ID(tileId) | ATTR2_PRIO(bgPrio) | ATTR2_PALBANK(palIndex),
            (bgPrio << 7) | objPrio);
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, GfxMode mode, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, flip, mode, false, objPrio);        
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, bool mosaic, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, flip, GfxMode::None, mosaic, objPrio);        
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, flip, GfxMode::None, false, objPrio);        
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, GfxMode mode, bool mosaic, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, mode, mosaic, objPrio);
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, GfxMode mode, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, mode, false, objPrio);        
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, bool mosaic, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, GfxMode::None, mosaic, objPrio);        
    }

    void pushRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex, int bgPrio, int objPrio = 0)
    {
        pushRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, GfxMode::None, false, objPrio);        
    }
};
