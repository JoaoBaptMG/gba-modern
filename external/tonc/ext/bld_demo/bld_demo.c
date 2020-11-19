//
// bld_demo.c
// testing blending
//
// (20031013 - 20060926, cearn)
//   bg 0, cbb  0, sbb 31, pb 15: text
//   bg 1, cbb  2, sbb 30, pb 1: metroid
//   bg 2, cbb  2, sbb 29, pb 0: fence
//   oam 0: tile 0-63: obj metroid

#include <stdio.h>
#include <tonc.h>
#include "metr.h"

void test_blend()
{
	tte_printf("#{P:48,8}obj#{P:168,8}bg");
	tte_set_margins(16, SCR_H-4-4*12, SCR_W-4, SCR_H-4);

	int mode=0;
	// eva, evb and ey are .4 fixeds
	// eva is full, evb and ey are empty
	u32 eva=0x80, evb= 0, ey=0;

	REG_BLDCNT= BLD_BUILD(
		BLD_OBJ | BLD_BG1,	// Top
		BLD_BG2,			// Bottom
		mode);				// Mode
	// also try this variation sometime:
	// REG_BLDCNT= BLD_BUILD(BLD_OBJ | BLD_BG1 |BLD_BG2, BLD_BG2 | BLD_BD, mode);

	while(1)
	{
		vid_vsync();
		key_poll();

		// interactive blend weights
		eva += key_tri_horz();
		evb -= key_tri_vert();
		ey  += key_tri_fire();
		
		mode += bit_tribool(key_hit(-1), KI_R, KI_L);

		// clamp to allowable ranges
		eva = clamp(eva, 0, 0x81);
		evb = clamp(evb, 0, 0x81);
		ey  = clamp(ey , 0, 0x81);
		mode= clamp(mode, 0, 4);

		tte_printf("#{es;P}mode :\t%2d\neva :\t%2d\nevb :\t%2d\ney :\t%2d",
			mode, eva/8, evb/8, ey/8);
		
		// Update blend mode
		BFN_SET(REG_BLDCNT, mode, BLD_MODE);

		// Update blend weights
		REG_BLDALPHA= BLDA_BUILD(eva/8, evb/8);
		REG_BLDY= BLDY_BUILD(ey/8);
	}	
}

void load_metr()
{
	// copy sprite and bg tiles, and the sprite palette
	memcpy32(&tile_mem[2][0], metrTiles, metrTilesLen/4);
	memcpy32(&tile_mem[4][0], metrTiles, metrTilesLen/4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen/4);

	// Set the metroid sprite
	obj_set_attr(&oam_mem[0], ATTR0_SQUARE | ATTR0_BLEND | ATTR0_Y(24), 
		ATTR1_SIZE_64 | ATTR1_X(32), 0);

	// Create the metroid bg using inverted palette for bg-metroid
	int ix, iy;
	for(ix=0; ix<16; ix++)
		pal_bg_mem[ix+16]= pal_obj_mem[ix] ^ CLR_WHITE;

	SCR_ENTRY *pse= &se_mem[30][3*32+18];	// right-center
	for(iy=0; iy<8; iy++)
		for(ix=0; ix<8; ix++)
			pse[iy*32+ix]= iy*8+ix + SE_PALBANK(1);

	REG_BG1CNT= BG_CBB(2) | BG_SBB(30);
}

// set-up the fence/text background
void load_fence()
{
	// tile 0 / ' ' will be a fence tile
	const TILE fence= 
	{{ 
		0x00012000, 0x00012000, 0x00022200, 0x22220222,
		0x11122211, 0x00112000, 0x00012000, 0x00012000,
	}};
	tile_mem[2][64]= fence;

	se_fill(se_mem[29], 64);

	pal_bg_mem[0]= RGB15(16, 10, 20);
	pal_bg_mem[1]= RGB15( 0,  0, 31);
	pal_bg_mem[2]= RGB15(16, 16, 16);

	REG_BG2CNT= BG_CBB(2) | BG_SBB(29);
}

int main()
{
	oam_init(oam_mem, 128);
	load_metr();
	load_fence();

	tte_init_chr4c_b4_default(0, BG_CBB(0)|BG_SBB(31));
	tte_init_con();

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | 
		DCNT_OBJ | DCNT_OBJ_1D;
	
	test_blend();

	return 0;
}
