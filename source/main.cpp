//--------------------------------------------------------------------------------
// main.cpp
//--------------------------------------------------------------------------------
// The main function
//--------------------------------------------------------------------------------
#include <tonc.h>

#include "scenes/IScene.hpp"
#include "gameplay/GameScene.hpp"
#include "scenes/TileTextTestScene.hpp"
#include "graphics/graphics.hpp"
//#include <maxmod.h>

//#include "soundbank.h"

//void maxmodInit();

#include "math/fixed.hpp"
#include "memory/allocator.hpp"

SceneStack scene EWRAM_BSS;
bool popScene EWRAM_BSS;

int main()
{
	// Enable interrupts
	irq_init(isr_master);
	irq_enable(II_VBLANK);
	//maxmodInit();

	// Init the memory allocator
	ewram::init();

	// Init the graphics module
	graphics::init();

	// Force a VBlank
	REG_DISPCNT = DCNT_BLANK;

	// Change to the main game scene
	scene.push<GameScene>();

	//mmStart(MOD_RPGLAST, MM_PLAY_ONCE);
	
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
		key_poll();
		//mmFrame();

		// Update the scene and change scenes if required
		scene.top()->update();
		if (popScene)
		{
			scene.pop();
			popScene = false;
		}
	}
}


