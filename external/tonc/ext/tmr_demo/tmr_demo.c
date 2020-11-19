//
// tmr_demo.c
// Demonstrating a cascade timer
// Using a slightly modified version of berk.gif from Headspin's fonts.
// Press start to start timer.
//
// (20031220 -20070809, Cearn)
 
#include <stdio.h>
#include <tonc.h>

#include "berk.h"


void tmr_test()
{
	// Overflow every ~1 second:
	// 0x4000 ticks @ FREQ_1024

	REG_TM2D= -0x4000;			// 0x4000 ticks till overflow
	REG_TM2CNT= TM_FREQ_1024;	// we're using the 1024 cycle timer	

	// cascade into tm3
	REG_TM3CNT= TM_ENABLE | TM_CASCADE;

	u32 sec= -1;

	while(1)
	{
		vid_vsync();
		key_poll();

		if(REG_TM3D != sec)
		{
			sec= REG_TM3D;
			tte_printf("#{es;P:24,60}%02d:%02d:%02d", 
				sec/3600, (sec%3600)/60, sec%60);
		}

		if(key_hit(KEY_START))	// pause by disabling timer
			REG_TM2CNT ^= TM_ENABLE;

		if(key_hit(KEY_SELECT))	// pause by enabling cascade
			REG_TM2CNT ^= TM_CASCADE;
	}
}

int main()
{
	// set-up berk font
	tte_init_se(0, BG_CBB(0)|BG_SBB(31), 1, 0, 0, &berkFont, se_drawg);
	tte_init_con();
	memcpy16(pal_bg_mem, berkPal, berkPalLen/4);

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	tmr_test();

	return 0;
}
