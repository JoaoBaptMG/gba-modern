//--------------------------------------------------------------------------------
// gamepak-removed.ewram.cpp
//--------------------------------------------------------------------------------
// The interrupt routine to be fired when the game pak is detected as removed
//--------------------------------------------------------------------------------
#include <tonc.h>
#include <array>
#include <algorithm>

#include "gamepak-removed.hpp"
#include "util/memswap.h"

// Licensing: the characters come fron the font Spleen, licensed under the BSD-2
// Copyright (c) 2018-2020, Frederic Cambus. https://github.com/fcambus/spleen
// The gamepak character is (c) 2020 PinoBatch (licensed under the CC0)

constexpr auto createFontSpriteArray()
{
    // Enum of letters
    enum Letters { _ = -1, A, D, E, G, H, I, K, L, M, N, O, P, R, S, T, V, W, PakData };

    // Data on the cart data
    constexpr auto PakWidth = 64, PakHeight = 32;
    constexpr auto PakSpacing = 16;

    // Data on the font
    constexpr auto FontWidth = 8, SpaceWidth = 6;
    constexpr auto LineHeight = 16, LineSpacing = 4;

    // Number of sprites
    std::array<OBJ_ATTR, 128> textSprites{};

    int y = (SCREEN_HEIGHT - (3 * LineHeight + 2 * LineSpacing + PakSpacing + PakHeight)) / 2;
    int x = (SCREEN_WIDTH - PakWidth) / 2;
    textSprites[0].attr0 = ATTR0_Y(y) | ATTR0_SQUARE;
    textSprites[0].attr1 = ATTR1_X(x) | ATTR1_SIZE_32x32;
    textSprites[0].attr2 = ATTR2_ID(2 * PakData) | ATTR2_PALBANK(0);

    x += PakWidth/2;
    textSprites[1].attr0 = ATTR0_Y(y) | ATTR0_SQUARE;
    textSprites[1].attr1 = ATTR1_X(x) | ATTR1_SIZE_32x32 | ATTR1_HFLIP;
    textSprites[1].attr2 = ATTR2_ID(2 * PakData) | ATTR2_PALBANK(0);

    y += PakHeight + PakSpacing;

    int i = 2;
    // Create the line plotter function
    auto createLine = [&](auto... chars)
    {
        int NumSpaces = ((chars == _) + ...);
        int NumChars = sizeof...(chars) - NumSpaces;
        x = (SCREEN_WIDTH - (NumChars * FontWidth + NumSpaces * SpaceWidth)) / 2;

        for (int ch : { chars... })
        {
            if (ch == _) x += SpaceWidth;
            else
            {
                textSprites[i].attr0 = ATTR0_Y(y) | ATTR0_TALL;
                textSprites[i].attr1 = ATTR1_X(x) | ATTR1_SIZE_8x16;
                textSprites[i].attr2 = ATTR2_ID(2*ch) | ATTR2_PALBANK(0);
                i++;
                x += FontWidth;
            }
        }

        y += LineHeight + LineSpacing;
    };

    // Set up the lines
    createLine(T,H,E,_,G,A,M,E,_,P,A,K,_,W,A,S,_,R,E,M,O,V,E,D);
    createLine(P,L,E,A,S,E,_,R,E,I,N,S,E,R,T,_,T,H,E,_,G,A,M,E,_,P,A,K);
    createLine(A,N,D,_,P,R,E,S,S,_,S,T,A,R,T);

    while (i < 128) textSprites[i++].attr0 = ATTR0_HIDE;

    return textSprites;
}

static auto TextSprites EWRAM_DATA = createFontSpriteArray();
extern u8 gamepakRemovedCharacters[1600] EWRAM_DATA;

static u32 dispCnt EWRAM_BSS;
static u16 bldCnt EWRAM_BSS;
static u16 bgColor EWRAM_BSS;
static u32 spriteColor EWRAM_BSS;

// Check for the data
#define TEST_TOKEN 0x47414d45
static const u32 TestData = TEST_TOKEN;

void gamepakRemovedIsr()
{
    // First, stop all DMA
    memset32((void*)(&REG_DMA0SAD), 0, 12);

    // Now, set the appropriate registers
    dispCnt = REG_DISPCNT;
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
    bldCnt = REG_BLDCNT;
    REG_BLDCNT = 0;

    // Set the background color to black
    bgColor = pal_bg_mem[0];
    pal_bg_mem[0] = 0;

    // Set the sprite colors
    spriteColor = *(vu32*)MEM_PAL_OBJ;
    *(vu32*)MEM_PAL_OBJ = CLR_WHITE << 16;

    // Now, set the sprite and background attributes
    memswap32(oam_mem, TextSprites.data(), TextSprites.size() * sizeof(OBJ_ATTR) / sizeof(u32));
    memswap32(tile_mem_obj, gamepakRemovedCharacters, sizeof(gamepakRemovedCharacters) / sizeof(u32));

    // Infinite loop - simple loop
    constexpr u32 AnimationLength = 16;
    u32 i = 0, anim = 0;
    while (true)
    {
        while(REG_DISPSTAT & DSTAT_IN_VBL) asm volatile("" ::: "memory");
        while(!(REG_DISPSTAT & DSTAT_IN_VBL)) asm volatile("" ::: "memory");

        i++;
        if (i == AnimationLength)
        {
            i = 0;
            anim = !anim;
            pal_obj_mem[1] = anim ? RGB15(27, 27, 27) : CLR_WHITE;
        }

        // Check if the start key is pressed (REG_KEYS is active-low)
        if (!(REG_KEYS & KEY_START))
            // Check for the test mirror - it's volatile, the compiler will not optimize it
            if (*const_cast<const volatile u32*>(&TestData) == TEST_TOKEN) break;
    }

    // Undo all the changes (but the DMA)
    memswap32(oam_mem, TextSprites.data(), TextSprites.size() * sizeof(OBJ_ATTR) / sizeof(u32));
    memswap32(tile_mem_obj, gamepakRemovedCharacters, sizeof(gamepakRemovedCharacters) / sizeof(u32));

    *(vu32*)MEM_PAL_OBJ = spriteColor;
    pal_bg_mem[0] = bgColor;
    REG_BLDCNT = bldCnt;
    REG_DISPCNT = dispCnt;
}
