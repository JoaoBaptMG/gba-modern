//
// sbb_aff.c
// showing screenblock boundaries for affine backgrounds
//
// (20031115 - 20080416, Cearn)
// === NOTES ===

#include <stdio.h>
#include <tonc.h>
#include "nums.h"

#define MAP_AFF_SIZE 0x0100

// --------------------------------------------------------------------
// GLOBALS
// --------------------------------------------------------------------

OBJ_ATTR *obj_cross= &oam_mem[0];
OBJ_ATTR *obj_disp= &oam_mem[1];

BG_AFFINE bgaff;

// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------

void win_textbox(int bgnr, int left, int top, int right, int bottom, int bldy)
{
	REG_WIN0H= left<<8 | right;
	REG_WIN0V=  top<<8 | bottom;
	REG_WIN0CNT= WIN_ALL | WIN_BLD;
	REG_WINOUTCNT= WIN_ALL;
	
	REG_BLDCNT= (BLD_ALL&~BIT(bgnr)) | BLD_BLACK;
	REG_BLDY= bldy;

	REG_DISPCNT |= DCNT_WIN0;

	tte_set_margins(left, top, right, bottom);
}

void init_cross()
{
	TILE cross= 
	{{  
		0x00011100, 0x00100010, 0x01022201, 0x01021201, 
		0x01022201, 0x00100010, 0x00011100, 0x00000000,
	}};
	tile_mem[4][1]= cross;

	pal_obj_mem[0x01]= pal_obj_mem[0x12]= CLR_WHITE;
	pal_obj_mem[0x02]= pal_obj_mem[0x11]= CLR_BLACK;

	obj_cross->attr2= 0x0001;
	obj_disp->attr2= 0x1001;
}

void init_map()
{
	int ii;

	memcpy32(&tile8_mem[0][1], nums8Tiles, nums8TilesLen/4);
	memcpy32(pal_bg_mem, numsPal, numsPalLen/4);

	REG_BG2CNT= BG_CBB(0) | BG_SBB(8) | BG_AFF_64x64;
	bgaff= bg_aff_default;

	// fill per 256 screen entries (=32x4 bands)
	u32 *pse= (u32*)se_mem[8];
	u32 ses= 0x01010101;
	for(ii=0; ii<16; ii++)
	{
		memset32(pse, ses, MAP_AFF_SIZE/4);
		pse += MAP_AFF_SIZE/4;
		ses += 0x01010101;
	}
}

void sbb_aff()
{
	AFF_SRC_EX asx= 
	{
		32<<8, 64<<8,			// Map coords.
		120, 80,				// Screen coords.
		0x0100, 0x0100, 0		// Scales and angle.
	};

	const int DX=256;
	FIXED ss= 0x0100;

	while(1)
	{
		vid_vsync();
		key_poll();

		// dir + A : move map in screen coords
		if(key_is_down(KEY_A))
		{
			asx.scr_x += key_tri_horz();
			asx.scr_y += key_tri_vert();
		}
		else	// dir : move map in map coords
		{
			asx.tex_x -= DX*key_tri_horz();
			asx.tex_y -= DX*key_tri_vert();
		}
		// rotate
		asx.alpha -= 128*key_tri_shoulder();

		
		// B: scale up ; B+Se : scale down
		if(key_is_down(KEY_B))
			ss += (key_is_down(KEY_SELECT) ? -1 : 1);

		// St+Se : reset
		// St : toggle wrapping flag.
		if(key_hit(KEY_START))
		{
			if(key_is_down(KEY_SELECT))
			{
				asx.tex_x= asx.tex_y= 0;
				asx.scr_x= asx.scr_y= 0;
				asx.alpha= 0;
				ss= 1<<8;
			}
			else					
				REG_BG2CNT ^= BG_WRAP;
		}

		asx.sx= asx.sy= (1<<16)/ss;

		bg_rotscale_ex(&bgaff, &asx);
		REG_BG_AFFINE[2]= bgaff;

		// the cross indicates the rotation point
		// (== p in map-space; q in screen-space)
		obj_set_pos(obj_cross, asx.scr_x-3, (asx.scr_y-3));
		obj_set_pos(obj_disp, (bgaff.dx>>8)-3, (bgaff.dy>>8)-3);

		tte_printf("#{es;P}p0\t: (%d, %d)\nq0\t: (%d, %d)\ndx\t: (%d, %d)", 
			asx.tex_x>>8, asx.tex_y>>8, asx.scr_x, asx.scr_y, 
			bgaff.dx>>8, bgaff.dy>>8);
	}	
}

int main()
{
	init_map();
	init_cross();

	REG_DISPCNT= DCNT_MODE1 | DCNT_BG0 | DCNT_BG2 | DCNT_OBJ;

	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();
	win_textbox(0, 8, 120, 232, 156, 8);

	sbb_aff();

	return 0;
}
