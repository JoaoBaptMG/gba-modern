//--------------------------------------------------------------------------------
// SaveSelectionScene.cpp
//--------------------------------------------------------------------------------
// Scene where the player can select one of the saves to load or overwrite
//--------------------------------------------------------------------------------
#include "SaveSelectionScene.hpp"
#include "SaveSelectionSceneDefs.hpp"

#include "colors.hpp"
#include "graphics/graphics.hpp"
#include "audio/audio.hpp"

#include "data/backgrounds/save-background.hpp"
#include "data/sprites/save-arrow.hpp"
#include "data/sounds/cursor.hpp"

constexpr auto BackgroundScreenblock = FirstSBlock - 1;

extern "C" void saveSelectionHblankIrq() IWRAM_CODE;
extern HBlankData *curHblankFrame IWRAM_CODE;

constexpr auto PaletteAnimationTime = 12;

// The first palette data
constexpr static const std::array ConstPalette = { colors::Black, colors::White, colors::Black, colors::DarkCyan };

constexpr static const std::array UnselectedPalette =
    { colors::rgb15(10, 0, 5), colors::rgb15(20, 0, 10), colors::rgb15(30, 0, 15) };

constexpr static const std::array SelectedPalette =
    { colors::rgb15(10, 10, 4), colors::rgb15(20, 20, 8), colors::rgb15(30, 30, 12) };

// The arrow
static StillImageAllocator image EWRAM_BSS(data::sprites::save_arrow.png.tiles, SpriteSize::s32x8_4bpp);
static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::save_arrow.png.palette);

constexpr int ArrowSeparation = PanelHeight/2 + 4;
constexpr int ArrowMovement = 4;
constexpr int NumArrowAnimationFrames = 64;

SaveSelectionScene::SaveSelectionScene() : IScene(), curAngle(0), targetAngle(0), curFrame(0), flipFrame(false),
    paletteAnim(0), arrowAnim(0), arrowPtr(image), palPtr(palette)
{
    // Set the background mode to 1 and enable background 2
    REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG2 | DCNT_OBJ;

    // Transfer the save panel data to the VRAM
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

    // Setup background 0
    REG_BG0CNT = BG_PRIO(2) | BG_CBB(1) | BG_4BPP | BG_SBB(BackgroundScreenblock) | BG_REG_32x32;

    // Transfer the background data
    data::copyTiles(&tile_mem[1], data::backgrounds::save_background.png);
    data::copyPalettes(&pal_bg_bank[15], data::backgrounds::save_background.png);

    // Copy all the screen entries
    constexpr const auto& bg = data::backgrounds::save_background.png;
    constexpr auto Width = bg.SeWidth;
    constexpr auto Height = bg.SeHeight;
    for (u32 i = 0; i < Height; i++)
        memcpy32(&se_mat[BackgroundScreenblock][i], &bg.scrEntries[i*Width], 
            Width*sizeof(SCR_ENTRY)/sizeof(u32));

    // Check the first palette
    pal_bg_mem[0] = bg.palettes[0][0];

    // Draw the first save panel
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
            audio::playSound(data::sounds::cursor.wav);
        }
        else if (key_is_down(KEY_DOWN))
        {
            transferPalette(false); paletteAnim = 0;
            if (curAngle == 0) curAngle = TotalFrames;
            targetAngle = curAngle - NumAnimFramesPerPanel;
            audio::playSound(data::sounds::cursor.wav);
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

    // Animate the arrows
    auto dy = ArrowSeparation;
    if (arrowAnim < NumArrowAnimationFrames)
        dy += ArrowMovement * arrowAnim / NumArrowAnimationFrames;
    else dy += ArrowMovement - ArrowMovement * (arrowAnim - NumArrowAnimationFrames) / NumArrowAnimationFrames;
    if (++arrowAnim == 2 * NumArrowAnimationFrames) arrowAnim = 0;

    // Position the arrows
    if (curAngle != targetAngle)
    {
        arrowUp.setHidden();
        arrowDown.setHidden();
    }
    else
    {
        arrowUp.setRegular(vec2(SCREEN_WIDTH / 2 - 16, SCREEN_HEIGHT / 2 - dy - 8), SpriteSize::s32x8_4bpp,
            arrowPtr.getTileId(), palPtr.getPalette(), 0, SpriteFlip::Vertical);
        arrowDown.setRegular(vec2(SCREEN_WIDTH / 2 - 16, SCREEN_HEIGHT / 2 + dy), SpriteSize::s32x8_4bpp,
            arrowPtr.getTileId(), palPtr.getPalette(), 0);
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
