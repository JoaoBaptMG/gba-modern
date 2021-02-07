//--------------------------------------------------------------------------------
// LevelSign.cpp
//--------------------------------------------------------------------------------
// Provides an animated introduction to the current level
//--------------------------------------------------------------------------------
#include "LevelSign.hpp"

#include <tonc.h>
#include "graphics/graphics.hpp"
#include "util/generateTable.hpp"
#include "math/gcem-degrees.hpp"
#include "UserInterfaceDefs.hpp"

constexpr auto MaxNumFrames = 240;
constexpr auto FadeInFrames = 128;
constexpr auto FadeOutFrames = 16;
constexpr auto MaxAlpha = 13;

constexpr auto MarkerWidthSpacing = 8 * (uidefs::LevelMarkerTileWidth + uidefs::LevelSignTileSpacing);

template <typename T, std::size_t N>
constexpr auto makeOffsets(const std::array<T, N>& numberWidths)
{
    std::array<s16, N> offsets{};
    for (std::size_t i = 0; i < N; i++)
        offsets[i] = -((SCREEN_WIDTH - MarkerWidthSpacing - numberWidths[i]) / 2);
    return offsets;
}

constexpr static const auto LevelOffsets = makeOffsets(std::array{ 28, 32 });

constexpr static const auto WobbleTable = generateTable<64>([](std::size_t i)
{
    return s16f<6>(gcem_d::sin(i * 360.0 / 18.0));
});

struct LevelSignData
{
    u32 charDataSize;
    const void* tiles;
    const u16* scrEntries;
    const u16* palettes;
};

#define BUILD_SIGN(v) { v.CharDataSize, v.tiles, v.scrEntries, v.palettes }
static const LevelSignData LevelSigns[] =
{
    BUILD_SIGN(data::backgrounds::level_numbers::_1.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_2.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_3.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_4.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_5.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_6.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_7.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_8.png),
    BUILD_SIGN(data::backgrounds::level_numbers::_9.png),
};

LevelSign::LevelSign(int level)
{
    const auto& curLevelSign = LevelSigns[level - 1];

    // Transfer the tiles to their designated space
    memcpy32(&UI_TILE_BANK[uidefs::SignTiles], data::backgrounds::level_mark.png.tiles,
        data::backgrounds::level_mark.png.CharDataSize / sizeof(u32));
    memcpy32(&UI_SIGN_PALETTE, curLevelSign.palettes, sizeof(PALBANK)/sizeof(u32));

    memcpy32(&UI_TILE_BANK[uidefs::SignTiles + uidefs::NumLevelTextTiles],
        curLevelSign.tiles, curLevelSign.charDataSize/sizeof(u32));

    // Set the number of frames
    numFrames = MaxNumFrames;

    // Set the DMA registers
    newDmaRegs[0] = (u32)hofsUpdates;
    newDmaRegs[1] = (u32)&REG_BG0HOFS;
    newDmaRegs[2] = 1 | DMA_SRC_INC | DMA_DST_RELOAD | DMA_AT_HBLANK | DMA_REPEAT | DMA_16 | DMA_ENABLE;
    restoreDmaRegister = 0;

    // Set the last update
    hofsUpdates[SignHeight] = 4;
    levelOffset = LevelOffsets[level != 1];

    // Set alpha-blending
    newBlendRegs[0] = BLD_BOT(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_TOP(BLD_BG0) | BLD_STD;

    // Set the blend register back to its place
    restoreBlendRegister = BLD_TOP(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_BLACK;

    // Set the vertical offset
    newVofsRegister = 0;
    restoreVofsRegister = 4;

    // Set the appropriate values
    u32 tileId = 0;
    for (int y = 0; y < uidefs::LevelSignTileHeight; y++)
        for (int x = 0; x < uidefs::LevelMarkerTileWidth; x++)
            UI_SCREEN[uidefs::SignTileBase + uidefs::TilePos(x, y)] = 
                data::backgrounds::level_mark.png.scrEntries[tileId++] + uidefs::SignTiles;
        
    tileId = 0;
    for (int y = 0; y < uidefs::LevelSignTileHeight; y++)
        for (int x = 0; x < uidefs::LevelNumberTileWidth; x++)
        {
            int xx = uidefs::LevelMarkerTileWidth + uidefs::LevelSignTileSpacing + x;
            UI_SCREEN[uidefs::SignTileBase + uidefs::TilePos(xx, y)] =
                curLevelSign.scrEntries[tileId++] + uidefs::SignTiles + uidefs::NumLevelTextTiles;
        }
}

LevelSign::~LevelSign()
{
    // Clear everything
    constexpr auto TileId = SE_PALBANK(15) | (uidefs::UserInterfaceTiles - 1);
    memset32(&UI_SCREEN[uidefs::SignTileBase], TileId | (TileId << 16), 
        32 * uidefs::LevelSignTileHeight * sizeof(SCR_ENTRY) / sizeof(u32));
}


void LevelSign::update()
{
    // Set the corresponding alpha
    u32 alpha = MaxAlpha;
    if (numFrames >= MaxNumFrames - FadeInFrames)
    {
        u32 frames = MaxNumFrames - numFrames;
        alpha = MaxAlpha * frames / FadeInFrames;

        // Set the wobble values appropriately
        if (frames > 0)
            for (u32 i = 0; i < SignHeight; i++)
            {
                u32 val = (FadeInFrames - frames) * (FadeInFrames - frames) / FadeInFrames / 4;
                hofsUpdates[i] = levelOffset + val * WobbleTable[(i + frames) % WobbleTable.size()];
            }
        else memset32(hofsUpdates, levelOffset | (levelOffset << 16), SignHeight * sizeof(s16) / sizeof(u32));
    }
    else if (numFrames < FadeOutFrames)
        alpha = MaxAlpha * numFrames / FadeOutFrames;

    newBlendRegs[1] = BLD_EVA(alpha) | BLD_EVB(16 - alpha);

    // Set the appropriate place for the register offset
    graphics::hblankEffects.add32((SCREEN_HEIGHT - SignHeight) / 2 - 1, newDmaRegs, (void*)&REG_DMA0SAD, 3);
    graphics::hblankEffects.add32((SCREEN_HEIGHT + SignHeight) / 2, &restoreDmaRegister, (void*)&REG_DMA0CNT, 1);

    // Set the vertical offset register
    graphics::hblankEffects.add16((SCREEN_HEIGHT - SignHeight) / 2 - 4, &newVofsRegister, (void*)&REG_BG0VOFS, 1);
    graphics::hblankEffects.add16((SCREEN_HEIGHT + SignHeight) / 2 + 2, &restoreVofsRegister, (void*)&REG_BG0VOFS, 1);

    // Set the blend register
    graphics::hblankEffects.add32((SCREEN_HEIGHT - SignHeight) / 2, newBlendRegs, (void*)&REG_BLDCNT, 1);
    graphics::hblankEffects.add16((SCREEN_HEIGHT + SignHeight) / 2 + 1, &restoreBlendRegister, (void*)&REG_BLDCNT, 1);

    numFrames--;
}
