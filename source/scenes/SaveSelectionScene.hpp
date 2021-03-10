//--------------------------------------------------------------------------------
// SaveSelectionScene.hpp
//--------------------------------------------------------------------------------
// Scene where the player can select one of the saves to load or overwrite
//--------------------------------------------------------------------------------
#pragma once

#if !__ASSEMBLER__
#include "IScene.hpp"
#include <tonc.h>
#include "math/stdfixed.hpp"
#include "graphics/StillImagePointer.hpp"
#include "graphics/OamManager.hpp"
#include "graphics/PalettePointer.hpp"
#endif

#include "memory/overlay.hpp"
#define SAVE_SELECTION_OVERLAY 1

#if !__ASSEMBLER__
struct HBlankData
{
    s32f8 x, y;
    s16f8 pa;
    u16 bgcnt;
};

class SaveSelectionScene final : public IScene
{
    u16 curAngle, targetAngle;
    u16 curFrame, flipFrame;
    u16 paletteAnim, arrowAnim;

    HBlankData hblankValues[2][SCREEN_HEIGHT];
    StillImagePointer arrowPtr;
    SinglePalettePointer palPtr;
    UniqueOamHandle arrowUp, arrowDown;

    void drawSavePanels(HBlankData* frame) IWRAM_OVERLAY_CODE(SAVE_SELECTION_OVERLAY);
    void transferPalette(bool selected);

    u32 currentPanel() const;
public:
    SaveSelectionScene();
    virtual void update() override;
    virtual void vblank() override;
    virtual ~SaveSelectionScene();
};
#endif
