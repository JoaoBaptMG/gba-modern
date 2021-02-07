//--------------------------------------------------------------------------------
// Background.cpp
//--------------------------------------------------------------------------------
// Class for showing a background
//--------------------------------------------------------------------------------
#include "Background.hpp"

#include <tonc.h>
#include "GameScene.hpp"

// Initialize the background
void Background::init()
{
    // Set BG1 as our background
    REG_BG1CNT = BG_CBB(0) | BG_SBB(30) | BG_REG_32x32 | BG_PRIO(1);

    offset = vec2<s32f8>();
}

// Load a new background
void Background::load(const DynamicBackgroundData& background)
{
    // Flip the 8bpp switch
    if (background.is8bpp) REG_BG1CNT |= BG_8BPP;
    else REG_BG1CNT &= ~BG_8BPP;

    // Load all the tiles to the first character block
    memcpy32(&tile_mem[0], background.tiles, background.charDataSize/sizeof(u32));

    // Special case for if the screen block is exactly 32 tiles long
    if (background.seWidth == 32)
        memcpy32(&se_mem[30], background.scrEntries, background.seHeight * (32*sizeof(u16)/sizeof(u32)));
    else // Pick up the slow way
    {
        for (u32 j = 0; j < background.seHeight; j++)
        {
            u32 numTiles = std::min<u32>(32, background.seWidth);
            memcpy16(&se_mem[30][32*j], &background.scrEntries[background.seWidth*j], numTiles);
        }
    }

    // Load the palette
    memcpy32(pal_bg_mem, background.palettes, background.numPalettes * (sizeof(PALBANK)/sizeof(u32)));
}

void Background::vblank()
{
    REG_BG1HOFS = offset.x;
    REG_BG1VOFS = offset.y;
}

GameScene& Background::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, background));
#pragma GCC diagnostic pop
}