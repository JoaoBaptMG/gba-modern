//
//  Oct test: getting the octant from coordinates
//
//! \file octtest.c
//! \author J Vijn
//! \date 20061001 - 20061001
//
// === NOTES ===

#include <stdio.h>
#include <tonc.h>

#include "octants.h"


#define OCT_STATE_ROT	1
#define OCT_PAL_START	1

void init_main()
{
	int ii;

	// --- irq ---
	irq_init(NULL);
	IRQ_SET(VBLANK);

	// --- bg ---
	GRIT_CPY(pal_bg_mem, octantsPal);
	for(ii=0; ii<8; ii++)
		pal_bg_mem[ii+OCT_PAL_START]= CLR_GRAY;
	LZ77UnCompVram(octantsTiles, tile_mem[0]);
	LZ77UnCompVram(octantsMap, se_mem[30]);

	REG_BG2CNT= BG_CBB(0) | BG_SBB(30) | BG_AFF_32x32;
	REG_BG_AFFINE[2]= bg_aff_default;

	// --- obj ---
	const TILE dot= 
	{{ 0, 0, 0x00011000, 0x00122100, 0x00122100, 0x00011000, 0, 0}};
	tile_mem[4][1]= dot;
	pal_obj_mem[1]= CLR_LIME;
	pal_obj_mem[2]= CLR_GREEN;
	obj_set_attr(&oam_mem[0], ATTR0_SQUARE, ATTR1_SIZE_8, 1);

	// --- text ----
	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();
	tte_set_margins(8, 8, 232, 40);

	REG_DISPCNT= DCNT_MODE1 | DCNT_BG0 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

int main()
{
	init_main();

	int x=12, y=8;	// Coords, relative to center.of screen
	u32 state= 0, oct_curr= 0, oct_prev= 0;
	OBJ_ATTR *dot= &oam_mem[0];

	AFF_SRC_EX asx= { 128<<8, 128<<8, 120, 80, 256, 256, 0};
	bg_rotscale_ex(&REG_BG_AFFINE[2], &asx);
	pal_bg_mem[oct_curr+OCT_PAL_START]= CLR_SILVER;

	while(1)
	{
		VBlankIntrWait();
		key_poll();

		x += key_tri_horz();
		y += key_tri_vert();

		obj_set_pos(dot, x+120-4, y+80-4);

		oct_curr= (state&OCT_STATE_ROT ? octant_rot(x, y) : octant(x,y) );

		if(oct_curr != oct_prev)
		{
			pal_bg_mem[oct_prev+OCT_PAL_START]= CLR_GRAY;
			pal_bg_mem[oct_curr+OCT_PAL_START]= CLR_SILVER;
			oct_prev= oct_curr;
		}
			
		if(key_hit(KEY_START))
		{
			state ^= OCT_STATE_ROT;
			asx.alpha ^= 0x1000;	// pi/8
			bg_rotscale_ex(&REG_BG_AFFINE[2], &asx);
		}

		tte_printf("#{es;P}(%d, %d) [%d]", x, y, oct_curr);
	}

	return 0;
}
