//--------------------------------------------------------------------------------
// graphics.cpp
//--------------------------------------------------------------------------------
// Provides functions to isolate the graphics operations
//--------------------------------------------------------------------------------

#include "graphics.hpp"
#include "util/gba-assert.hpp"

constexpr auto MaxObjs = 128;
constexpr auto MaxCopies = 32;
constexpr auto MaxVerticals = 8;

static OBJ_ATTR shadowOAM[MaxObjs] ALIGN4;

constexpr u32 MaxCopyWords = 256;
constexpr u32 MaxVerticalWords = 64;

constexpr u32 MaxObjTiles = 1024;

static u32 copyBuffer[MaxCopyWords] IWRAM_DATA;
static u32 verticalBuffer[MaxVerticalWords] IWRAM_DATA;

static u32 objCount IWRAM_DATA, copyCount IWRAM_DATA, verticalCount IWRAM_DATA;
static u32 tileCount EWRAM_BSS, palettesUsed EWRAM_BSS;

void graphics::init()
{
    objCount = 0;
    copyCount = 0;
    verticalCount = 0;
    tileCount = 0;
    palettesUsed = 0;

    oam_init(shadowOAM, MaxObjs);
}

void graphics::update()
{
    // Do all the DMA commands
    u32* ptr = copyBuffer;
    while (ptr < copyBuffer+copyCount)
    {
        auto cnt = *ptr++;
        auto dst = *ptr++;
        REG_DMA3SAD = (u32)ptr;
        REG_DMA3DAD = dst;
        REG_DMA3CNT = cnt;
        ptr += u16(cnt);
    }
    copyCount = 0;

    // Do all the vertical commands
    ptr = verticalBuffer;
    while (ptr < verticalBuffer+verticalCount)
    {
        auto cnt = *ptr++;
        auto dst = (u32*)*ptr++;
        while (cnt--)
        {
            *dst = *ptr++;
            dst += 16;
        }
    }
    verticalCount = 0;

    // Copy the shadow OAM
    oam_copy(oam_mem, shadowOAM, MaxObjs);

    // "Reset" the OAM by hiding all objects
    while (objCount)
    {
        objCount--;
        shadowOAM[objCount].attr0 = ATTR0_HIDE;
    }
}

void graphics::pushOAM(u16 attr0, u16 attr1, u16 attr2)
{
    ASSERT(objCount < MaxObjs);
    shadowOAM[objCount].attr0 = attr0;
    shadowOAM[objCount].attr1 = attr1;
    shadowOAM[objCount].attr2 = attr2;
    objCount++;
}

void* graphics::newCopyCommand32(void* dst, u16 count)
{
    ASSERT(copyCount + count + 2 <= MaxCopyWords);
    copyBuffer[copyCount++] = count | DMA_32NOW | DMA_ENABLE;
    copyBuffer[copyCount++] = (u32)dst;
    copyCount += count;
    return copyBuffer + (copyCount-count);
}

void* graphics::newVerticalCopyCommand32(void* dst, u16 count)
{
    ASSERT(verticalCount + count + 2 <= MaxVerticalWords);
    verticalBuffer[verticalCount++] = count;
    verticalBuffer[verticalCount++] = (u32)dst;
    verticalCount += count;
    return verticalBuffer + (verticalCount-count);
}

void graphics::resetObjectsAndPalettes()
{
    tileCount = 0;
    palettesUsed = 0;
}

u32 graphics::allocateObjTiles(u32 numTiles)
{
    ASSERT(tileCount + numTiles <= MaxObjTiles);
    auto oldTileCount = tileCount;
    tileCount += numTiles;
    return oldTileCount;
}

void graphics::allocateObjPalettes(u32 numPalettes, u32* indices)
{
    // Allocate a single palette at a time
    for (u32 i = 0; i < 16; i++)
    {
        if (palettesUsed & (1 << i)) continue;
        palettesUsed |= 1 << i;
        *indices++ = i;
        if (--numPalettes == 0) return;
    }

    // Not all palettes were preallocated
    ASSERT(false);
}

void graphics::freeObjPalettes(u32 numPalettes, const u32* indices)
{
    // Free all palettes
    for (u32 j = 0; j < numPalettes; j++)
    {
        u32 i = *indices++;
        palettesUsed &= ~(1 << i);
    }
}