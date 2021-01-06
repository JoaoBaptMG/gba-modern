//--------------------------------------------------------------------------------
// graphics.cpp
//--------------------------------------------------------------------------------
// Provides functions to isolate the graphics operations
//--------------------------------------------------------------------------------
#include "graphics.hpp"
#include "BuddyObjectAllocator.hpp"
#include "util/gba-assert.hpp"

// TODO: remove this - for debugging purposes
#define CLEAR_PALETTE_AFTER

constexpr u32 MaxCopyWords = 256;
constexpr u32 MaxVerticalWords = 64;
constexpr u32 MaxRomCopies = 32;

static u32 copyBuffer[MaxCopyWords] IWRAM_DATA;
static u32 verticalBuffer[MaxVerticalWords] IWRAM_DATA;
struct RomCopy { const void* src; void* dst; u32 countCtl; };
static RomCopy romCopyBuffer[MaxRomCopies] EWRAM_BSS;

static u16 copyCount EWRAM_BSS, verticalCount EWRAM_BSS, romCopyCount EWRAM_BSS;
static u16 palettesUsed EWRAM_BSS, palettesToFree EWRAM_BSS;

static BuddyObjectAllocator buddy EWRAM_BSS;
OamManager graphics::oam IWRAM_DATA;
HblankEffects graphics::hblankEffects IWRAM_DATA;

void graphics::init()
{
    copyCount = 0;
    verticalCount = 0;
    romCopyCount = 0;
    palettesUsed = 0;
    palettesToFree = 0;
    oam.init();
    hblankEffects.init();
}

void graphics::vblank()
{
    // Update the hblank
    hblankEffects.vblank();

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

    // Copy to shadow OAM
    oam.copyToOAM();
}

void graphics::update()
{
    // Commit the free tiles and the free palettes so we do not get the nasty bug
    buddy.commitFreeBlocks();
    commitFreePalettes();
}

void graphics::postUpdate()
{
    oam.sortOAM();
    hblankEffects.flush();
}

void* graphics::newCopyCommand32(void* dst, u16 count)
{
    ASSERT(copyCount + count + 2u <= MaxCopyWords);
    copyBuffer[copyCount++] = count | DMA_32NOW | DMA_ENABLE;
    copyBuffer[copyCount++] = (u32)dst;
    copyCount += count;
    return copyBuffer + (copyCount-count);
}

void* graphics::newVerticalCopyCommand32(void* dst, u16 count)
{
    ASSERT(verticalCount + count + 2u <= MaxVerticalWords);
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
        palettesToFree |= 1 << i;
    }
}

void graphics::commitFreePalettes()
{
#ifdef CLEAR_PALETTE_AFTER
    for (u32 i = 0; i < 16; i++)
    {
        if (palettesToFree & (1 << i))
            memset32(pal_obj_bank+i, 0, sizeof(PALBANK) / sizeof(u32));
    }
#endif

    palettesUsed &= ~palettesToFree;
    palettesToFree = 0;
}