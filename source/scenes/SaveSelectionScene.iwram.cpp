//--------------------------------------------------------------------------------
// SaveSelectionScene.iwram.cpp
//--------------------------------------------------------------------------------
// Scene where the player can select one of the saves to load or overwrite
//--------------------------------------------------------------------------------
#include "SaveSelectionScene.hpp"
#include "SaveSelectionSceneDefs.hpp"

#include "math/gcem-degrees.hpp"
#include "math/fixedmath.hpp"
#include "util/generateTable.hpp"
#include "text/mGBADebugging.hpp"

// Details for the perspective transform
constexpr s32 FocalDistance = 196;

using fdata = s32f8;

// Generate the sin table
constexpr auto makePanelTable()
{
    std::array<fdata, TotalFrames> out{};
    for (std::size_t i = 0; i < TotalFrames; i++)
        out[i] = fdata(PanelHeight * gcem_d::sin(360.0 * i / TotalFrames));
    return out;
}

constexpr static const std::array<fdata, TotalFrames> PanelSinTable = makePanelTable();

constexpr u32 whileWrap(u32 x, u32 w)
{
    while (x >= w) x -= w;
    return x;
}

// Crude modulo without invoking modulo
constexpr fdata panelSin(u32 x)
{
    return PanelSinTable[whileWrap(x, TotalFrames)];
}

void SaveSelectionScene::drawSavePanels(HBlankData* frame)
{
    // Get the point values
    fdata yp[6], zp[6], ysp[6];
    for (u32 i = 0; i < 6; i++)
    {
        u32 angle = curAngle + i * TotalFrames/6;
        yp[i] = panelSin(angle + 11*TotalFrames/12);
        zp[i] = FocalDistance + PanelSinTable[TotalFrames/6] - panelSin(angle + TotalFrames/6);
        ysp[i] = yp[i] * fdata(FocalDistance) / zp[i];
    }

    // Get the last one to be drawn
    u32 last = currentPanel();
    for (u32 k = last+1; k <= last+6; k++)
    {
        u32 i = whileWrap(k, 6);
        u32 in = whileWrap(k+1, 6);

        // Check if we should draw them
        int ymini = ysp[i], ymaxi = ysp[in];
        if (ymini >= ymaxi) continue;

        // Clamp it to the height of the screen
        ymini = std::clamp(ymini, -SCREEN_HEIGHT/2, SCREEN_HEIGHT/2-1);
        ymaxi = std::clamp(ymaxi, -SCREEN_HEIGHT/2, SCREEN_HEIGHT/2-1);

        frame[SCREEN_HEIGHT/2 + ymini].bgcnt = BG_PRIO(0) | BG_CBB(0) | BG_8BPP | BG_SBB(FirstSBlock+i) | BG_AFF_16x16;

        // Precompute some stuff
        auto mult = yp[in] * zp[i] - zp[in] * yp[i];
        auto yif = yp[i] * FocalDistance;
        auto dyif = (yp[in] - yp[i]) * FocalDistance;
        auto dzi = zp[in] - zp[i];

        mgba::log(mgba::Log::Info, i, ": mult = ", int(mult), " / ymini = ", ymini, " / ymaxi = ", ymaxi);

        // Now, compute for each scanline the necessary lines to be drawn
        for (int ys = ymini; ys <= ymaxi; ys++)
        {
            u32 j = SCREEN_HEIGHT/2 + ys;
            frame[j].y = -(PanelHeight * (yif - zp[i] * ys) / (dyif - dzi * ys));
            fdata z = FocalDistance * (mult / (dyif - dzi * ys));
            frame[j].x = (PanelWidth - z * SCREEN_WIDTH / FocalDistance) / 2;
            frame[j].pa = z / FocalDistance;
        }
    }

    mgba::log(mgba::Log::Info, "--------");

    flipFrame = true;
}