//
// pageflip.c
// Shows mode4 page flipping
//
// (20031003 - 20060922, Cearn)

#include <string.h>

#include "toolbox.h"
#include "page_pic.h"

void load_gfx()
{
	int ii;
	for(ii=0; ii<16; ii++)
	{
		memcpy(&vid_mem_front[ii*120], &frontBitmap[ii*144/4], 144);
		memcpy(&vid_mem_back[ii*120], &backBitmap[ii*144/4], 144);
	}

	// You don't have to do everything with memcpy.
	// In fact, for small blocks it might be better if you didn't.
	// Just mind your types, though. No sense in copying from a 32bit 
	// array to a 16bit one.
	u32 *dst= (u32*)pal_bg_mem;
	for(ii=0; ii<8; ii++)
		dst[ii]= frontPal[ii];
}

int main()
{
	load_gfx();
	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;

	int ii=0;

	while(1)
	{		
		while(KEY_DOWN_NOW(KEY_START)) ;	// pause with start

		vid_vsync();
		if(++ii == 60)
		{	
			ii=0;	
			vid_flip();	
		}
	}
	
	return 0;
}
