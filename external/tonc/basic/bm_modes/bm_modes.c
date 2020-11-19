//
// bm_modes.c
// Combined demo of modes 3, 4 and 5
//
// (20031002 - 20060922, cearn)

#include <string.h>
#include "toolbox.h"
#include "modes.h"

int main()
{
	int mode= 3;
	REG_DISPCNT= mode | DCNT_BG2;

	// Copy the data and palette to the right
	// addresses
	memcpy(vid_mem, modesBitmap, modesBitmapLen);
	memcpy(pal_bg_mem, modesPal, modesPalLen);

	while(1)
	{
		// Wait till VBlank before doing anything
		vid_vsync();

		// Check keys for mode change
		key_poll();
		if(key_hit(KEY_LEFT) && mode>3)
			mode--;
		else if(key_hit(KEY_RIGHT) && mode<5)
			mode++;

		// Change the mode
		REG_DISPCNT= mode | DCNT_BG2;
	}

	return 0;
}
