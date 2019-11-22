//--------------------------------------------------------------------------------
// graphics.cpp
//--------------------------------------------------------------------------------
// Provides functions to isolate the graphics operations
//--------------------------------------------------------------------------------

#include "graphics.hpp"
#include "graphics/BuddyObjectAllocator.hpp"
#include "util/gba-assert.hpp"

constexpr auto MaxObjs = 128;
constexpr u32 MaxCopyWords = 256;
constexpr u32 MaxVerticalWords = 64;
constexpr u32 MaxRomCopies = 32;

static OBJ_ATTR shadowOAM[MaxObjs] ALIGN4;

static u32 copyBuffer[MaxCopyWords] IWRAM_DATA;
static u32 verticalBuffer[MaxVerticalWords] IWRAM_DATA;
struct RomCopy { const void* src; void* dst; u32 countCtl; };
static RomCopy romCopyBuffer[MaxRomCopies] IWRAM_DATA;

static u32 objCount IWRAM_DATA, copyCount IWRAM_DATA, verticalCount IWRAM_DATA, romCopyCount IWRAM_DATA;
static u32 palettesUsed EWRAM_BSS;

static BuddyObjectAllocator buddy EWRAM_BSS;

void graphics::init()
{
    objCount = 0;
    copyCount = 0;
    verticalCount = 0;
    romCopyCount = 0;
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

    // Do all the copy commands
    for (uint i = 0; i < romCopyCount; i++)
    {
        REG_DMA3SAD = (u32)romCopyBuffer[i].src;
        REG_DMA3DAD = (u32)romCopyBuffer[i].dst;
        REG_DMA3CNT = romCopyBuffer[i].countCtl;
    }
    romCopyCount = 0;

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

void graphics::romCopyCommand32(void* dst, const void* src, u16 count)
{
    ASSERT(romCopyCount < MaxCopyWords);
    romCopyBuffer[romCopyCount].dst = dst;
    romCopyBuffer[romCopyCount].src = src;
    romCopyBuffer[romCopyCount].countCtl = count | DMA_32NOW | DMA_ENABLE;
    romCopyCount++;
}

void graphics::resetObjectsAndPalettes()
{
    palettesUsed = 0;
}

u16 graphics::allocateObjTiles(int blocks)
{
    return buddy.allocBlock(blocks);
}

void graphics::freeObjTiles(u16 blocks)
{
    buddy.freeBlock(blocks);
}

void graphics::allocateObjPalettes(u32 numPalettes, u16* indices)
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

void graphics::freeObjPalettes(u32 numPalettes, const u16* indices)
{
    // Free all palettes
    for (u32 j = 0; j < numPalettes; j++)
    {
        u32 i = *indices++;
        palettesUsed &= ~(1 << i);
    }
}