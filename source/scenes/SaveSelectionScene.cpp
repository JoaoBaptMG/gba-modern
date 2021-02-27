//--------------------------------------------------------------------------------
// SaveSelectionScene.cpp
//--------------------------------------------------------------------------------
// Scene where the player can select one of the saves to load or overwrite
//--------------------------------------------------------------------------------
#include "SaveSelectionScene.hpp"

#include "data/backgrounds/save-panel.hpp"
#include "math/gcem-degrees.hpp"
#include "math/fixedmath.hpp"
#include "util/generateTable.hpp"
#include "text/mGBADebugging.hpp"

constexpr auto NumSaves = 6;
constexpr auto FirstSBlock = 32 - NumSaves;
constexpr auto ScreenblockSize = sizeof(data::backgrounds::save_panel.png.scrEntries) / NumSaves;
constexpr auto PanelWidth = data::backgrounds::save_panel.png.SeWidth * 8;
constexpr auto PanelHeight = data::backgrounds::save_panel.png.SeHeight / NumSaves * 8;

constexpr auto NumAnimFramesPerPanel = 32;
constexpr auto TotalFrames = NumAnimFramesPerPanel * NumSaves;

using fdata = s32f8;

// Details for the perspective transform
constexpr s32 FocalDistance = 128;
constexpr fdata InvFocalDistance = 1.0 / FocalDistance;

// Generate the sin table
constexpr auto makePanelTable()
{
    std::array<fdata, TotalFrames> out{};
    for (std::size_t i = 0; i < TotalFrames; i++)
        out[i] = fdata(PanelHeight * gcem_d::sin(360.0 * i / TotalFrames));
    return out;
}

constexpr static const std::array<fdata, TotalFrames> PanelSinTable = makePanelTable();

extern "C" void saveSelectionHblankIrq() IWRAM_CODE;
extern HBlankData *curHblankFrame IWRAM_CODE;

SaveSelectionScene::SaveSelectionScene() : IScene(), curAngle(0), targetAngle(0), curFrame(0), flipFrame(false)
{
    // Set the background mode to 1 and enable background 2
    REG_DISPCNT = DCNT_MODE1 | DCNT_BG2;

    // Configure background 2
    REG_BG2CNT = BG_PRIO(0) | BG_CBB(0) | BG_8BPP | BG_SBB(FirstSBlock) | BG_AFF_16x16;

    // Transfer the data to the VRAM
    data::copyTiles(&tile_mem[0], data::backgrounds::save_panel.png);
    data::copyPalettes(pal_bg_mem, data::backgrounds::save_panel.png);

    // Transfer each one of the screenblocks to their respective places
    for (u32 i = 0; i < NumSaves; i++)
        memcpy32(&se_mem[FirstSBlock]+i, 
            &data::backgrounds::save_panel.png.scrEntries[i * ScreenblockSize], 
            ScreenblockSize/sizeof(u32));

    drawSavePanels();

    // Install the hblank interrupt
    irq_add(II_HBLANK, saveSelectionHblankIrq);
    irq_enable(II_HBLANK);
}

// Crude modulo without invoking modulo
constexpr fdata GetPanelSin(u32 x)
{
    if (x >= 2 * TotalFrames) x -= 2 * TotalFrames;
    else if (x >= TotalFrames) x -= TotalFrames;
    return PanelSinTable[x];
}

constexpr fdata GetPanelCos(u32 x)
{
    return GetPanelSin(x + TotalFrames/4);
}

void SaveSelectionScene::update()
{
    // Unfill the frame
    auto frame = &hblankValues[curFrame][0];
    memset32(frame, 0, SCREEN_HEIGHT * sizeof(HBlankData) / sizeof(u32));

    if (curAngle == targetAngle)
    {
        if (key_is_down(KEY_UP))
            targetAngle = curAngle + NumAnimFramesPerPanel;
        else if (key_is_down(KEY_DOWN))
        {
            if (curAngle == 0) curAngle = TotalFrames;
            targetAngle = curAngle - NumAnimFramesPerPanel;
        }
    }

    if (curAngle < targetAngle)
    {
        if (++curAngle == TotalFrames)
            curAngle = targetAngle = 0;
        drawSavePanels();
    }
    else if (curAngle > targetAngle)
    {
        curAngle--;
        drawSavePanels();
    }

}

void SaveSelectionScene::drawSavePanels()
{
    auto frame = &hblankValues[curFrame][0];

    for (u32 i = 0; i < 6; i++)
    {
        u32 angle = curAngle + i*TotalFrames/6;

        // Get the point coordinates
        fdata yi = GetPanelSin(angle + 11*TotalFrames/12);
        fdata dyi = GetPanelSin(angle + TotalFrames/4);
        
        fdata zi = FocalDistance + PanelSinTable[TotalFrames/6] - GetPanelSin(angle + TotalFrames/6);
        fdata dzi = GetPanelSin(angle);

        // Precompute the multiplier and dyi*f
        auto mult = dyi * zi - dzi * yi;
        fdata yif = yi * FocalDistance;
        fdata dyif = dyi * FocalDistance;

        // Calculate the y in screen space
        auto ysi = yi * fdata(FocalDistance) / zi;
        auto ysin = (yi + dyi) * fdata(FocalDistance) / (zi + dzi);

        // Compute, the minimum and the maximum
        int ymini = ysi;
        int ymaxi = ysin;

        // Don't draw if it is upside down
        if (ymini >= ymaxi) continue;

        // Clamp the values to the screen
        ymini = std::clamp(ymini, -SCREEN_HEIGHT/2, SCREEN_HEIGHT/2-1);
        ymaxi = std::clamp(ymaxi, -SCREEN_HEIGHT/2, SCREEN_HEIGHT/2-1);

        frame[SCREEN_HEIGHT/2 + ymini].bgcnt = BG_PRIO(0) | BG_CBB(0) | BG_8BPP | BG_SBB(FirstSBlock+i) | BG_AFF_16x16;

        // Now, compute for each scanline the necessary lines to be drawn
        for (int ys = ymini; ys <= ymaxi; ys++)
        {
            u32 j = SCREEN_HEIGHT/2 + ys;
            frame[j].y = -(PanelHeight * (yif - zi * ys) / (dyif - dzi * ys));
            fdata z = FocalDistance * (mult / (dyif - dzi * ys));

            frame[j].x = (PanelWidth - z * SCREEN_WIDTH * InvFocalDistance) / 2;
            frame[j].pa = z * InvFocalDistance;
        }
    }

    flipFrame = true;
}

void SaveSelectionScene::vblank()
{
    // Flip the current frame only modifications were done
    if (flipFrame)
    {
        flipFrame = false;
        curHblankFrame = &hblankValues[curFrame][0];
        curFrame = !curFrame;
    }
}

SaveSelectionScene::~SaveSelectionScene()
{
    // Disable HBlank irq
    irq_disable(II_HBLANK);
}
