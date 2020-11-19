//
//! \file txt_se1.c
//!   Screen-entry text demo
//! \date 20051128 - 20060930
//! \author cearn
//
// === NOTES ===

#include <tonc.h>

#define LEGACY_TEXT_V11
#include <tonc_legacy.h>

#include "border.h"


// --------------------------------------------------------------------
// CONSTANTS & STRUCTS
// --------------------------------------------------------------------


#define TID_FRAME0        96
#define TID_FRAME1       105
#define TID_FONT           0
#define TID_FONT2        128
#define TID_FONT3        256
#define TXT_PID_SHADE   0xEE
#define TXT_PID_BG      0x88


// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------


void init_main()
{
	int ii;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	txt_init_std();
	txt_init_se(0, BG_CBB(0) | BG_SBB(31), 0x1000, CLR_RED, 0x0E);

	// again, with a twist
	txt_init_se(0, BG_CBB(0) | BG_SBB(31), 0xF000|TID_FONT2, 
		CLR_YELLOW | (CLR_MAG<<16), TXT_PID_SHADE);

	// and once more, with feeling!
	txt_init_se(0, BG_CBB(0) | BG_SBB(31), 0xE000|TID_FONT3,
		0, TXT_PID_SHADE);
	u32 *pwd= (u32*)&tile_mem[0][TID_FONT3];
	for(ii=0; ii<96*8; ii++)
		*pwd++ |= quad8(TXT_PID_BG);

	// extra border initialisation
	memcpy32(&tile_mem[0][TID_FRAME0], borderTiles, borderTilesLen/4);
	memcpy32(pal_bg_mem, borderPal, borderPalLen/4);

	// overwrite /\ [] `% ^_ to use border tiles
	//  / ^ \ 
	//  [ # ] 
	//  ` _ '  
	const u8 bdr_lut[9]= "/^\\[#]`_\'";
	for(ii=0; ii<9; ii++)
		gptxt->chars[bdr_lut[ii]]= TID_FRAME0+ii;

	// set some extra colors
	pal_bg_mem[0x1F]= CLR_RED;
	pal_bg_mem[0x2F]= CLR_GREEN;
	pal_bg_mem[0x3F]= CLR_BLUE;

	pal_bg_mem[0xE8]= pal_bg_mem[0x08];	// bg
	pal_bg_mem[0xEE]= CLR_ORANGE;		// shadow
	pal_bg_mem[0xEF]= pal_bg_mem[0x0F];	// text
}

void txt_se_frame(int l, int t, int r, int b, u16 se0)
{
	int ix, iy;
	u8 *lut= gptxt->chars;
	u16 *pse= (u16*)gptxt->dst0;
	pse += t*32 + l;
	r -= (l+1);
	b -= (t+1);

	// corners
	pse[32*0  + 0] = se0+lut['/'];
	pse[32*0  + r] = se0+lut['\\'];
	pse[32*b  + 0] = se0+lut['`'];
	pse[32*b  + r] = se0+lut['\''];

	// horizontal
	for(ix=1; ix<r; ix++)
	{
		pse[32*0+ix]= se0+lut['^'];
		pse[32*b+ix]= se0+lut['_'];
	}
	// vertical + inside
	pse += 32;
	for(iy=1; iy<b; iy++)
	{
		pse[0]= se0+lut['['];
		pse[r]= se0+lut[']'];
		for(ix=1; ix<r; ix++)
			pse[ix]= se0+lut['#'];
		pse += 32;
	}
}

int main()
{
	init_main();

	// red, green, blue text
	se_puts(8, 16, "bank 1:\n  red",   0x1000);
	se_puts(8, 40, "bank 2:\n  green", 0x2000);
	se_puts(8, 72, "bank 3:\n  blue",  0x3000);
	se_puts(8, 96, "bank 15:\n yellow, \nwith mag \nshadow", 
		0xF000|TID_FONT2);

	// framed text, v1
	txt_se_frame(10, 2, 29, 9, 0);
	se_puts( 88, 24, "frame 0:", 0);
	se_puts(104, 32, "/^\\[#]`_'", 0);
	se_puts( 88, 40, "bank  0:\n  basic text,\n  transparent bg", 
		0);

	// framed text, v2
	txt_se_frame(10, 11, 29, 18, TID_FRAME1-TID_FRAME0);
	se_puts( 88,  96, "frame 1:", 0xE000|TID_FONT3);
	se_puts(104, 104, "/^\\[#]`_'", 9);
	se_puts( 88, 112, "bank 14:\n  shaded text\n  opaque bg", 
		0xE000|TID_FONT3);

	while(1)
	{
		VBlankIntrWait();
	}

	return 0;
}
