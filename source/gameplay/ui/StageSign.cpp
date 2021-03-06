//--------------------------------------------------------------------------------
// StageSign.cpp
//--------------------------------------------------------------------------------
// Provides an animated introduction to the current stage
//--------------------------------------------------------------------------------
#include "StageSign.hpp"

#include <tonc.h>
#include "graphics/graphics.hpp"
#include "util/generateTable.hpp"
#include "math/gcem-degrees.hpp"
#include "UserInterfaceDefs.hpp"

constexpr auto Offset = -4;
constexpr auto MaxNumFrames = 240;
constexpr auto FadeInFrames = 128;
constexpr auto FadeOutFrames = 16;
constexpr auto MaxAlpha = 13;

constexpr auto MarkerWidthSpacing = 8 * (uidefs::StageMarkerTileWidth + uidefs::StageSignTileSpacing);

template <typename T, std::size_t N>
constexpr auto makeOffsets(const std::array<T, N>& numberWidths)
{
    std::array<s16, N> offsets{};
    for (std::size_t i = 0; i < N; i++)
        offsets[i] = -((SCREEN_WIDTH - MarkerWidthSpacing - numberWidths[i]) / 2);
    return offsets;
}

constexpr static const auto StageOffsets = makeOffsets(std::array{ 28, 32 });

constexpr static const auto WobbleTable = generateTable<64>([](std::size_t i)
{
    return s16f<6>(gcem_d::sin(i * 360.0 / 18.0));
});

struct StageSignData
{
    u32 tileDataSize;
    const void* tiles;
    const SCR_ENTRY* scrEntries;
    const PALBANK* palettes;
};

#define BUILD_SIGN(v) { sizeof(v.tiles), v.tiles, v.scrEntries, v.palettes }
static const StageSignData StageSigns[] =
{
    BUILD_SIGN(data::backgrounds::stage_numbers::_1.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_2.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_3.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_4.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_5.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_6.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_7.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_8.png),
    BUILD_SIGN(data::backgrounds::stage_numbers::_9.png),
};

StageSign::StageSign(int stage)
{
    const auto& curStageSign = StageSigns[stage - 1];

    // Transfer the tiles to their designated space
    data::copyTiles(&UI_TILE_BANK[uidefs::SignTiles], data::backgrounds::stage_mark.png);
    memcpy32(&UI_SIGN_PALETTE, curStageSign.palettes, sizeof(PALBANK)/sizeof(u32));
    memcpy32(&UI_TILE_BANK[uidefs::SignTiles + uidefs::NumStageTextTiles],
        curStageSign.tiles, curStageSign.tileDataSize / sizeof(u32));

    // Set the number of frames
    numFrames = MaxNumFrames;
    curStage = stage;

    // Set the DMA registers
    newDmaRegs[0] = (u32)hofsUpdates;
    newDmaRegs[1] = (u32)&REG_BG0HOFS;
    newDmaRegs[2] = 1 | DMA_SRC_INC | DMA_DST_RELOAD | DMA_AT_HBLANK | DMA_REPEAT | DMA_16 | DMA_ENABLE;
    restoreDmaRegister = 0;

    // Set the last update
    hofsUpdates[SignHeight] = 4;
    stageOffset = StageOffsets[stage != 1];

    // Set alpha-blending
    newBlendRegs[0] = BLD_BOT(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_TOP(BLD_BG0) | BLD_STD;

    // Set the blend register back to its place
    restoreBlendRegister = BLD_TOP(BLD_BACKDROP | BLD_OBJ | BLD_BG3 | BLD_BG2 | BLD_BG1) | BLD_BLACK;
}

StageSign::~StageSign()
{
    // Clear everything
    constexpr auto TileId = SE_PALBANK(15) | (uidefs::UserInterfaceTiles - 1);
    memset32(&UI_SCREEN[uidefs::SignTileBase], TileId | (TileId << 16), 
        32 * uidefs::StageSignTileHeight * sizeof(SCR_ENTRY) / sizeof(u32));
}

void StageSign::setStageScreenEntries()
{
    const auto& curStageSign = StageSigns[curStage - 1];

    // Set the appropriate values
    u32 tileId = 0;
    for (u32 y = 0; y < uidefs::StageSignTileHeight; y++)
        for (u32 x = 0; x < uidefs::StageMarkerTileWidth; x++)
            UI_SCREEN[uidefs::SignTileBase + uidefs::TilePos(x, y)] = 
                data::backgrounds::stage_mark.png.scrEntries[tileId++] + uidefs::SignTiles;
        
    tileId = 0;
    for (u32 y = 0; y < uidefs::StageSignTileHeight; y++)
        for (u32 x = 0; x < uidefs::StageNumberTileWidth; x++)
        {
            u32 xx = uidefs::StageMarkerTileWidth + uidefs::StageSignTileSpacing + x;
            UI_SCREEN[uidefs::SignTileBase + uidefs::TilePos(xx, y)] =
                curStageSign.scrEntries[tileId++] + uidefs::SignTiles + uidefs::NumStageTextTiles;
        }
}

void StageSign::update()
{
    // Set the screen entries only before
    if (numFrames == MaxNumFrames - 1) setStageScreenEntries();

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
                hofsUpdates[i] = stageOffset + val * WobbleTable[(i + frames) % WobbleTable.size()];
            }
        else memset32(hofsUpdates, stageOffset | (stageOffset << 16), SignHeight * sizeof(s16) / sizeof(u32));
    }
    else if (numFrames < FadeOutFrames)
        alpha = MaxAlpha * numFrames / FadeOutFrames;

    newBlendRegs[1] = BLD_EVA(alpha) | BLD_EVB(16 - alpha);

    constexpr auto TopVal = (SCREEN_HEIGHT - SignHeight) / 2 + Offset;
    constexpr auto BotVal = (SCREEN_HEIGHT + SignHeight) / 2 + Offset;

    // Set the appropriate place for the register offset
    graphics::hblankEffects.add32(TopVal - 1, newDmaRegs, (void*)&REG_DMA0SAD, 3);
    graphics::hblankEffects.add32(BotVal, &restoreDmaRegister, (void*)&REG_DMA0CNT, 1);

    // Set the blend register
    graphics::hblankEffects.add32(TopVal, newBlendRegs, (void*)&REG_BLDCNT, 1);
    graphics::hblankEffects.add16(BotVal + 1, &restoreBlendRegister, (void*)&REG_BLDCNT, 1);

    numFrames--;
}
