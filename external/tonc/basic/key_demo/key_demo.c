//
// key_demo.c
//
// demonstating key states (and minor palette animation)
//
// (20030927 - 20060922, cearn)

#include <string.h>

#include "toolbox.h"
#include "input.h"

#include "gba_pic.h"

#define BTN_PAL  5
#define CLR_UP   RGB15(27,27,29)


int main()
{
	int ii;
	u32 btn;
	COLOR clr;
	int frame=0;

	memcpy(vid_mem, gba_picBitmap, gba_picBitmapLen);
	memcpy(pal_bg_mem, gba_picPal, gba_picPalLen);

	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;

	while(1)
	{
		vid_vsync();
		// slowing down polling to make the changes visible
		if((frame & 7) == 0)
			key_poll();
		// check state of each button
		for(ii=0; ii<KI_MAX; ii++)
		{
			clr=0;
			btn= 1<<ii;
			if(key_hit(btn))
				clr= CLR_RED;
			else if(key_released(btn))
				clr= CLR_YELLOW;
			else if(key_held(btn))
				clr= CLR_LIME;
			else
				clr= CLR_UP;
			pal_bg_mem[BTN_PAL+ii]= clr;
		}
		frame++;
	}

	return 0;
}
