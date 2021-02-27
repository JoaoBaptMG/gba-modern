//--------------------------------------------------------------------------------
// SaveSelectionScene.hpp
//--------------------------------------------------------------------------------
// Scene where the player can select one of the saves to load or overwrite
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"
#include <tonc.h>
#include "math/stdfixed.hpp"

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

    HBlankData hblankValues[2][SCREEN_HEIGHT];

    void drawSavePanels();

public:
    SaveSelectionScene();
    virtual void update() override;
    virtual void vblank() override;
    virtual ~SaveSelectionScene();
};
