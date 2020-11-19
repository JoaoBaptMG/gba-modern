//
// mos_demo.c
// testing a mosaic sprite and background
//
// (20031013 - 20071203, cearn)
// === NOTES ===
// * bg/oam use:
//   bg 0, cbb  0, sbb 31, pb 0: text
//   bg 1, cbb  1, sbb 30, pb 1: bg metroid
//   oam 0: tile 0-63: obj metroid

#include <stdio.h>
#include <tonc.h>
#include "metr.h"

void test_mosaic()
{
	tte_printf("#{P:48,8}obj#{P:168,8}bg");
	tte_set_margins(4, SCR_H-30, SCR_W-4, SCR_H-4);

	POINT pt_obj={0,0}, pt_bg={0,0};
	POINT *ppt= &pt_obj;

	while(1)
	{
		vid_vsync();

		// control the mosaic
		key_poll();

		// switch between bg or obj mosaic
		ppt= key_is_down(KEY_A) ? &pt_bg : &pt_obj;

		ppt->x += key_tri_horz();		// inc/dec h-mosaic
		ppt->y -= key_tri_vert();		// inc/dec v-mosaic

		ppt->x= clamp(ppt->x, 0, 0x80);
		ppt->y= clamp(ppt->y, 0, 0x80);

		REG_MOSAIC= MOS_BUILD(pt_bg.x>>3, pt_bg.y>>3, 
			pt_obj.x>>3, pt_obj.y>>3);

		tte_printf("#{es;P}obj h,v: %2d,%2d\n bg h,v: %2d,%2d", 
			pt_obj.x>>3, pt_obj.y>>3, pt_bg.x>>3, pt_bg.y>>3);
	}
}

void load_metr()
{
	int ix, iy;

	memcpy32(&tile_mem[1][0], metrTiles, metrTilesLen/4);
	memcpy32(&tile_mem[4][0], metrTiles, metrTilesLen/4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen/4);

	// create object: oe0
	OBJ_ATTR *metr= &oam_mem[0];
	obj_set_attr(metr, ATTR0_SQUARE | ATTR0_MOSAIC, ATTR1_SIZE_64, 0);		
	obj_set_pos(metr, 32, 24);				// left-center

	// create bg map: bg1, cbb1, sbb 31
	for(ix=1; ix<16; ix++)
		pal_bg_mem[ix+16]= pal_obj_mem[ix] ^ CLR_WHITE;

	SCR_ENTRY *pse= &se_mem[30][3*32+18];	// right-center
	for(iy=0; iy<8; iy++)
		for(ix=0; ix<8; ix++)
			pse[iy*32+ix]= (iy*8+ix) | SE_PALBANK(1);

	REG_BG1CNT= BG_CBB(1) | BG_SBB(30) | BG_MOSAIC;
}

int main()
{
	// setup sprite
	oam_init(oam_mem, 128);

	load_metr();

	// set-up text: bg0, cbb0, sbb31
	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(31));
	tte_init_con();

	REG_DISPCNT= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
	
	test_mosaic();

	return 0;
}
