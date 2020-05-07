//--------------------------------------------------------------------------------
// main.cpp
//--------------------------------------------------------------------------------
// The main function
//--------------------------------------------------------------------------------
#include <tonc.h>

#include "scenes/IScene.hpp"
#include "gameplay/GameScene.hpp"
#include "scenes/BitmaskTestScene.hpp"
#include "graphics/graphics.hpp"
#include "math/fixed.hpp"
#include "memory/allocator.hpp"
#include "text/mGBADebugging.hpp"
#include "audio/audio.hpp"

SceneStack scene EWRAM_BSS;
bool popScene EWRAM_BSS;

int main()
{
    // Enable interrupts
    irq_init(isr_master);
    irq_enable(II_VBLANK);

    // Init the memory allocator
    ewram::init();

    // Init the graphics module
    graphics::init();

    // Init the audio module
    audio::init();

    // Force a VBlank
    REG_DISPCNT = DCNT_BLANK;

    mgba::enable();
    // Change to the main game scene
    scene.push<GameScene>();

    while (1)
    {
        // The effect of this loop is the following: if the program manages
        // to meet its processor schedule, VBlankIntrWait will call and wait
        // for the next VBlank. If, however, it fails to do so, the call will
        // assure that the important vblank routines only get called at the
        // _next_ vblank
        VBlankIntrWait();
        graphics::vblank();
        scene.top()->vblank();

        graphics::update();
        audio::mix();
        key_poll();

        // Update the scene and change scenes if required
        scene.top()->update();
        if (popScene)
        {
            scene.pop();
            popScene = false;
        }

        if (mgba::isEnabled())
        {
            auto vcount = REG_VCOUNT;
            if (vcount >= 160) vcount -= 160;
            else vcount += 228 - 160;

            // 25/57 is equal to 100/228 aka 100% * vcount * number of lines in a frame
            mgba::log(mgba::Log::Debug, "estimated CPU load: ", 25 * vcount / 57, "%");
        }
    }
}


