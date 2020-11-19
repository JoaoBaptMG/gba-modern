//
// dma_demo.c
// Circular window via HDMA
//
// (20031210 - 20070808, Cearn)
// === NOTES ===

#include <stdio.h>
#include <tonc.h>

#include "brin.h"

u16 g_winh[SCREEN_HEIGHT+1];

//! Create an array of horizontal offsets for a circular window.
/*!	The offsets are to be copied to REG_WINxH each HBlank, either 
*	  by HDMA or HBlank isr. Offsets provided by modified 
*	  Bresenham's circle routine (of course); the clipping code is not
*	  optional.
*	\param winh	Pointer to array to receive the offsets.
*	\param x0	X-coord of circle center.
*	\param y0	Y-coord of circle center.
*	\param rr	Circle radius.
*/
void win_circle(u16 winh[], int x0, int y0, int rr)
{
	int x=0, y= rr, d= 1-rr;
	u32 tmp;

	// clear the whole array first.
	memset16(winh, 0, SCREEN_HEIGHT+1);

	while(y >= x)
	{
		// Side octs
		tmp  = clamp(x0+y, 0, SCREEN_WIDTH+1);
		tmp += clamp(x0-y, 0, SCREEN_WIDTH+1)<<8;
		
		if(IN_RANGE(y0-x, 0, SCREEN_HEIGHT))		// o4, o7
			winh[y0-x]= tmp;
		if(IN_RANGE(y0+x, 0, SCREEN_HEIGHT))		// o0, o3
			winh[y0+x]= tmp;

		// Change in y: top/bottom octs
		if(d >= 0)		
		{
			tmp  = clamp(x0+x, 0, SCREEN_WIDTH+1);
			tmp += clamp(x0-x, 0, SCREEN_WIDTH+1)<<8;
			
			if(IN_RANGE(y0-y, 0, SCREEN_HEIGHT))	// o5, o6
				winh[y0-y]= tmp;
			if(IN_RANGE(y0+y, 0, SCREEN_HEIGHT))	// o1, o2
				winh[y0+y]= tmp;

			d -= 2*(--y);
		}
		d += 2*(x++)+3;
	}
	winh[SCREEN_HEIGHT]= winh[0];
}

void init_main()
{
	// --- Init BG 2 (main bg) ---
	dma3_cpy(pal_bg_mem, brinPal, brinPalLen);
	dma3_cpy(tile_mem[0], brinTiles, brinTilesLen);
	dma3_cpy(se_mem[30], brinMap, brinMapLen);

	//memcpy32(pal_bg_mem, brinPal, brinPalLen/4);
	//memcpy32(tile_mem[0], brinTiles, brinTilesLen/4);
	//memcpy32(se_mem[30], brinMap, brinMapLen/4);

	REG_BG2CNT= BG_CBB(0)|BG_SBB(30) | BG_REG_64x32;
	REG_BG2HOFS= 120;
	REG_BG2VOFS= 64;

	// --- Init BG 1 (mask) ---
	const TILE tile= 
	{{
		0xF2F3F2F3, 0x3F2F3F2F, 0xF3F2F3F2, 0x2F3F2F3F, 
		0xF2F3F2F3, 0x3F2F3F2F, 0xF3F2F3F2, 0x2F3F2F3F
	}};
	tile_mem[0][32]= tile;
	pal_bg_bank[4][ 2]= RGB15(12,12,12);
	pal_bg_bank[4][ 3]= RGB15( 8, 8, 8);
	pal_bg_bank[4][15]= RGB15( 0, 0, 0);
	se_fill(se_mem[29], 0x4020);

	REG_BG1CNT= BG_CBB(0)|BG_SBB(29);

	// --- Init BG 0 (text) ---
	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();
	tte_set_margins(8, 8, 232, 40);

	// --- Init window ---
	REG_WIN0H= 0;
	REG_WIN0V= SCREEN_HEIGHT;

	// --- Enable stuff ---
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_WIN0;
	REG_WININ= WIN_BUILD(WIN_BG0|WIN_BG2, 0);
	REG_WINOUT= WIN_BUILD(WIN_BG0|WIN_BG1, 0);
}

int main()
{
	int rr=64, x0=120, y0=80;

	init_main();

	while(1)
	{
		vid_vsync();
		key_poll();

		rr += key_tri_shoulder();	// size with B/A
		x0 += key_tri_horz();		// move left/right
		y0 += key_tri_vert();		// move up/down

		if(rr<0)
			rr= 0;

		// Fill circle array
		win_circle(g_winh, x0, y0, rr);
	
		// Init win-circle HDMA
		DMA_TRANSFER(&REG_WIN0H, &g_winh[1], 1, 3, DMA_HDMA);

		tte_printf("#{es;P}%d %d | %d ", x0, y0, rr);
	}
	
	return 0;
} 

