//--------------------------------------------------------------------------------
// SaveSelectionScene.cpp
//--------------------------------------------------------------------------------
// Scene where the player can select one of the saves to load or overwrite
//--------------------------------------------------------------------------------
#include "SaveSelectionScene.hpp"
#include "SaveSelectionSceneDefs.hpp"

#include "colors.hpp"
#include "graphics/graphics.hpp"

extern "C" void saveSelectionHblankIrq() IWRAM_CODE;
extern HBlankData *curHblankFrame IWRAM_CODE;

constexpr auto PaletteAnimationTime = 12;

// The first palette data
constexpr static const std::array ConstPalette = { colors::Black, colors::White, colors::Black, colors::DarkCyan };

constexpr static const std::array UnselectedPalette =
    { colors::rgb15(10, 0, 5), colors::rgb15(20, 0, 10), colors::rgb15(30, 0, 15) };

constexpr static const std::array SelectedPalette =
    { colors::rgb15(10, 10, 4), colors::rgb15(20, 20, 8), colors::rgb15(30, 30, 12) };

SaveSelectionScene::SaveSelectionScene() : IScene(), curAngle(0), targetAngle(0), curFrame(0), flipFrame(false),
    paletteAnim(0)
{
    // Set the background mode to 1 and enable background 2
    REG_DISPCNT = DCNT_MODE1 | DCNT_BG2;

    // Transfer the data to the VRAM
    data::copyTiles(&tile_mem[0], data::backgrounds::save_panel.png);
    memcpy32(pal_bg_mem, ConstPalette.data(), sizeof(ConstPalette)/sizeof(u32));

    // Transfer each one of the screenblocks to their respective places
    for (u32 i = 0; i < NumSaves; i++)
    {
        memcpy32(&se_mem[FirstSBlock]+i, 
            &data::backgrounds::save_panel.png.scrEntries[i * ScreenblockSize], 
            ScreenblockSize/sizeof(u32));

        u32 j = 0;
        for (auto color : UnselectedPalette)
            pal_bg_mem[ConstPalette.size() + 3*i + j++] = color;
    }

    drawSavePanels(&hblankValues[curFrame][0]);

    // Install the hblank interrupt
    irq_add(II_HBLANK, saveSelectionHblankIrq);
    irq_enable(II_HBLANK);
}

void SaveSelectionScene::update()
{
    // Unfill the frame
    auto frame = &hblankValues[curFrame][0];
    memset32(frame, 0, SCREEN_HEIGHT * sizeof(HBlankData) / sizeof(u32));

    if (curAngle == targetAngle)
    {
        if (key_is_down(KEY_UP))
        {
            transferPalette(false); paletteAnim = 0;
            targetAngle = curAngle + NumAnimFramesPerPanel;
        }
        else if (key_is_down(KEY_DOWN))
        {
            transferPalette(false); paletteAnim = 0;
            if (curAngle == 0) curAngle = TotalFrames;
            targetAngle = curAngle - NumAnimFramesPerPanel;
        }

        // Do a quick palette animation
        if (paletteAnim == PaletteAnimationTime)
            transferPalette(true);
        else if (paletteAnim == 2*PaletteAnimationTime)
        {
            transferPalette(false);
            paletteAnim = 0;
        }
        paletteAnim++;
    }

    if (curAngle < targetAngle)
    {
        if (++curAngle == TotalFrames)
            curAngle = targetAngle = 0;
        drawSavePanels(frame);
    }
    else if (curAngle > targetAngle)
    {
        curAngle--;
        drawSavePanels(frame);
    }

}

void SaveSelectionScene::transferPalette(bool selected)
{
    // Request some data to write
    auto ptr = selected ? SelectedPalette.data() : UnselectedPalette.data();
    graphics::romCopyCommand16(&pal_bg_mem[ConstPalette.size() + 3*currentPanel()],
        ptr, sizeof(SelectedPalette)/sizeof(u16));
}

u32 SaveSelectionScene::currentPanel() const
{
    if (curAngle == 0) return 0;
    return (TotalFrames - curAngle) / NumAnimFramesPerPanel;
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
