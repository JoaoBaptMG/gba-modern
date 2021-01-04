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

// Important enums
enum class SpriteFlip { None, Horizontal, Vertical, Both };
enum class GfxMode { None, AlphaBlending, Window };

constexpr auto NoObj = (u8)255;

class UniqueOamHandle final
{
    u8 handle;
    void setAttrs(u16 attr0, u16 attr1, u16 attr2, u16 prio = 0) const;

    struct NoOamTag {};
    UniqueOamHandle(NoOamTag) : handle(NoObj) {}

public:
    static inline auto noObj() { return UniqueOamHandle(NoOamTag{}); }

    UniqueOamHandle();
    ~UniqueOamHandle();

    // Disable copying
    UniqueOamHandle(const UniqueOamHandle&) = delete;
    UniqueOamHandle& operator=(const UniqueOamHandle&) = delete;

    UniqueOamHandle(UniqueOamHandle&& other);
    UniqueOamHandle& operator=(UniqueOamHandle&& other);

    OBJ_ATTR& operator*() const;
    OBJ_ATTR* operator->() const { return &operator*(); }

    void setHidden() const
    {
        auto& obj = operator*();
        obj.attr0 = (obj.attr0 & ~ATTR0_MODE_MASK) | ATTR0_HIDE;
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, GfxMode mode, bool mosaic, int objPrio = 0) const
    {
        setAttrs((pos.y & 255) | (int(mode) << 10) | (mosaic ? ATTR0_MOSAIC : 0) | SizeUtils::attr0param(size),
            (pos.x & 511) | (int(flip) << 12) | SizeUtils::attr1param(size),
            ATTR2_ID(tileId) | ATTR2_PRIO(bgPrio) | ATTR2_PALBANK(palIndex),
            (bgPrio << 7) | objPrio);
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, GfxMode mode, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, flip, mode, false, objPrio);        
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, bool mosaic, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, flip, GfxMode::None, mosaic, objPrio);        
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, SpriteFlip flip, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, flip, GfxMode::None, false, objPrio);        
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, GfxMode mode, bool mosaic, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, mode, mosaic, objPrio);
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, GfxMode mode, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, mode, false, objPrio);        
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex,
        int bgPrio, bool mosaic, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, GfxMode::None, mosaic, objPrio);        
    }

    void setRegular(vec2<int> pos, SpriteSize size, int tileId, int palIndex, int bgPrio, int objPrio = 0) const
    {
        setRegular(pos, size, tileId, palIndex, bgPrio, SpriteFlip::None, GfxMode::None, false, objPrio);        
    }
};

class OamManager final
{
    // The required array and size
    u32 objCount, prevObjCount;
    alignas(void*) OBJ_ATTR shadowOAM[MaxObjs]; // This is stored and persistent
    u8 pos[MaxObjs], idByPos[MaxObjs];          // position on the shadow OAM table
    u32 firstFreePos;                           // the first free position

    u8 newHandle();
    void freeHandle(u8 handle);

public:
    OamManager() {}
    void init();
    void sortOAM() IWRAM_CODE;
    void copyToOAM();

    u32 size() const { return objCount; }

    friend class UniqueOamHandle;
};
