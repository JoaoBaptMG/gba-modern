//
// oacombo.c
// Affine object combo's, with the off-center rotations
//   that go with it.
//
// (20060419 - 20070808, cearn)

#include <stdio.h>
#include <tonc.h>

#include "oac_gfx.h"

#define AX	(16<<8)		// X-anchor
#define AY	(16<<8)		// Y-anchor
#define X0	 120		// base X
#define Y0	 36			// base Y	

void obj_rs_ex2(OBJ_ATTR *obj, OBJ_AFFINE *oa, AFF_SRC_EX *asx);

typedef struct OACOMBO
{
	OBJ_ATTR *sub_obj;		// obj pointer for sub-objects
	POINT *sub_pos;			// Local sub-object coords (.8f)
	int sub_count;			// Number of sub-objects
	POINT pos;				// Global position (.8f)
	POINT anchor;			// Local anchor (.8f)
	s16 sx, sy;
	u16 alpha;
} OACOMBO;


// === GLOBALS ========================================================

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;


// Obj templates
const OBJ_ATTR obj_data[7]=
{
	// obj[0]  , oa[0]: 1 full 32x32p double-affine circle
	{ 0x0300, 0x8200, 0x0000, 0x0000 }, 
	// obj[1-2], oa[1]: 2 32x16p double-affine semi-circles 
	{ 0x4300, 0x8200, 0x0000, 0x0000 }, 
	{ 0x4300, 0x8200, 0x0008, 0x0000 }, 
	// obj[3-7], oa[1]: 4 16x16p double-affine quarter-circles
	{ 0x0300, 0x4400, 0x0010, 0x0000 }, 
	{ 0x0300, 0x4400, 0x0014, 0x0000 }, 
	{ 0x0300, 0x4400, 0x0018, 0x0000 }, 
	{ 0x0300, 0x4400, 0x001C, 0x0000 }, 
};

POINT sub_pos[7]=
{
	{0,0},
	{0,0},{0,AY}, 
	{0,0},{AX,0}, {0,AY},{AX,AY}, 
};

OACOMBO oacs[3]=
{
	// full 32x32p double-affine circle
	{ &obj_buffer[0], &sub_pos[0], 1, 
		{(X0-48)<<8, Y0<<8}, {AX, AY}, 256, 256, 0 },
	// 2 32x16p double-affine semi-circles
	{ &obj_buffer[1], &sub_pos[1], 2, 
		{(X0+16)<<8, Y0<<8}, {AX, AY}, 256, 256, 0 },
	// 4 16x16p double-affine quarter-circles
	{ &obj_buffer[3], &sub_pos[3], 4, 
		{(X0-16)<<8, (Y0+40)<<8}, {AX, AY}, 256, 256, 0 },
};


// === FUNCTIONS ======================================================

void oac_rotscale(OACOMBO *oac)
{
	// --- create P ---
	int sx= oac->sx, sy= oac->sy;
	int sina= lu_sin(oac->alpha)>>4, cosa= lu_cos(oac->alpha)>>4;
	
	OBJ_AFFINE *oaff= 
		&obj_aff_buffer[BFN_GET(oac->sub_obj->attr1, ATTR1_AFF_ID)];
	oaff->pa=  cosa*sx>>8;	oaff->pb= -sina*sx>>8;
	oaff->pc=  sina*sy>>8;	oaff->pd=  cosa*sy>>8;

	// sx = 1/sx, sy = 1/sy (.12f)
	sx= Div(1<<20, sx);
	if(sx != sy)
		sy= Div(1<<20, sy);
	else
		sy= sx;
	FIXED aa, ab, ac, ad;
	aa=  sx*cosa>>12;	ab= sy*sina>>12;	// .8f
	ac= -sx*sina>>12;	ad= sy*cosa>>12;	// .8f

	int ii;
	OBJ_ATTR *obj= oac->sub_obj;
	POINT *pt= oac->sub_pos;
	for(ii=0; ii<oac->sub_count; ii++)
	{
		int dx, dy;		// all .8f
		sx= oam_sizes[obj->attr0>>14][obj->attr1>>14][0]<<7;
		sy= oam_sizes[obj->attr0>>14][obj->attr1>>14][1]<<7;
		
		dx= oac->pos.x+oac->anchor.x - sx;	// .8f
		dy= oac->pos.y+oac->anchor.y - sy;	// .8f
		
		if(obj->attr0&ATTR0_AFF_DBL_BIT)
		{	dx -= sx;	dy -= sy;	}

		sx= oac->anchor.x - pt->x - sx;
		sy= oac->anchor.y - pt->y - sy;

		dx -= (aa*sx + ab*sy)>>8;		// .8f
		dy -= (ac*sx + ad*sy)>>8;		// .8f
		obj_set_pos(obj, dx>>8, dy>>8);	

		obj++;	pt++;
	}
}

void init_main()
{
	memcpy32(pal_obj_mem, oac_gfxPal, oac_gfxPalLen/4);
	memcpy32(tile_mem[4], oac_gfxTiles, oac_gfxTilesLen/4);

	oam_init(obj_buffer, 128);

	// init objs and obj combos
	memcpy32(obj_buffer, obj_data, sizeof(obj_data)/4);

	REG_DISPCNT= DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();

	// Some labels
	tte_printf("#{P:%d,%d}1 full #{P:%d,%d}2 semi #{P:%d,%d}4 quarts", 
		X0-48, Y0-16, X0+20, Y0-16, X0-20, Y0+74);
}


int main()
{
	init_main();

	int ii, alpha=0;

	while(1)
	{
		vid_vsync();
		key_poll();
		alpha -= 128*key_tri_shoulder();

		for(ii=0; ii<3; ii++)
		{
			oacs[ii].alpha= alpha;
			oac_rotscale(&oacs[ii]);
		}
		oam_copy(oam_mem, obj_buffer, 128);
	}
	return 0;
}


// alternate main; uses obj_rs_ex()
int alt_main()
{
	init_main();

	AFF_SRC_EX asxs[7]= 
	{
		// 1 32x32p double-affine circle
		{ 16<<8, 16<<8, 32, 36, 256, 256, 0 },
		// 2 32x16p double-affine semi-circles
		{ 16<<8, 16<<8, 96, 36, 256, 256, 0 },
		{ 16<<8,  0   , 96, 36, 256, 256, 0 },
		// 4 16x16p double-affine quarter-circles
		{ 16<<8, 16<<8, 64, 76, 256, 256, 0 },
		{  0   , 16<<8, 64, 76, 256, 256, 0 },
		{ 16<<8,  0   , 64, 76, 256, 256, 0 },
		{  0   ,  0   , 64, 76, 256, 256, 0 },
	};

	int ii, alpha=0;
	
	while(1)
	{
		vid_vsync();
		key_poll();
		alpha -= 128*key_tri_shoulder();

		for(ii=0; ii<7; ii++)
		{
			asxs[ii].alpha= alpha;
			obj_rs_ex2(&obj_buffer[ii], &obj_aff_buffer[ii], &asxs[ii]);
		}

		oam_copy(oam_mem, obj_buffer, 128);
	}

	return 0;
}

