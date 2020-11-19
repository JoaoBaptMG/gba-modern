//
// obj_aff.c
// Fun with affine transformations
// 
// Note that the code is quite complex due to all the 
// different things you can do.
//
// (20031010 - 20070808, cearn)

#include <stdio.h>
#include <tonc.h>

#include "metr.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;


// affine transformation constants and variables
enum eAffState
{
	AFF_NULL=0, AFF_ROTATE, AFF_SCALE_X, AFF_SCALE_Y, 
	AFF_SHEAR_X, AFF_SHEAR_Y, AFF_COUNT
};

// 'speeds' of transformations
const int aff_diffs[AFF_COUNT]= { 0, 128, 4, 4, 4, 4 };

// keys for transformation direction
const int aff_keys[AFF_COUNT]=
{ 0, KEY_L, KEY_SELECT, KEY_SELECT, KEY_RIGHT, KEY_UP };
int aff_state= AFF_NULL;
int aff_value= 0;

// === FUNCTIONS ======================================================

void init_metr()
{
	// Places the tiles of a 4bpp metroid sprite into LOW obj VRAM
	memcpy32(tile_mem[4], metr_boxTiles, metr_boxTilesLen/4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen/4);

	// setup main metroid
	obj_set_attr(obj_buffer, 
		ATTR0_SQUARE | ATTR0_AFF,		// Square Aff-sprite
		ATTR1_SIZE_64 | ATTR1_AFF_ID(0),	// 64x64, using obj_aff[0]
		0 | 0);							// palbank 0, tile 0
	obj_set_pos(obj_buffer, 96, 32);
	obj_aff_identity(&obj_aff_buffer[0]);

	// setup shadow metroid
	obj_set_attr(&obj_buffer[1], 
		ATTR0_SQUARE | ATTR0_AFF,		// Square Aff-sprite
		ATTR1_SIZE_64 | ATTR1_AFF_ID(31),	// 64x64, using obj_aff[0]
		ATTR2_PALBANK(1) | 0);			// palbank 1, tile 0
	obj_set_pos(&obj_buffer[1], 96, 32);
	obj_aff_identity(&obj_aff_buffer[31]);

	oam_copy(oam_mem, obj_buffer, 128);
}

int get_aff_state()
{
	if(key_is_down(KEY_L | KEY_R))
		return AFF_ROTATE;
	if(key_is_down(KEY_A))
		return AFF_SCALE_X;
	if(key_is_down(KEY_B))
		return AFF_SCALE_Y;
	if(key_is_down(KEY_LEFT | KEY_RIGHT))
		return AFF_SHEAR_X;
	if(key_is_down(KEY_UP | KEY_DOWN))
		return AFF_SHEAR_Y;
	return AFF_NULL;
}

void get_aff_new(OBJ_AFFINE *oaff)
{
	int diff= aff_diffs[aff_state];
	aff_value += (key_is_down(aff_keys[aff_state]) ? diff : -diff);

	switch(aff_state)
	{
	case AFF_ROTATE:	// L rotates left, R rotates right
		//aff_value &= SIN_MASK;
		obj_aff_rotate(oaff, aff_value);
		break;
	case AFF_SCALE_X:	// A scales x, +SELECT scales down
		obj_aff_scale_inv(oaff, (1<<8)-aff_value, 1<<8);
		break;
	case AFF_SCALE_Y:	// B scales y, +SELECT scales down
		obj_aff_scale_inv(oaff, 1<<8, (1<<8)-aff_value);
		break;
	case AFF_SHEAR_X:	// shear left and right
		obj_aff_shearx(oaff, aff_value);
		break;
	case AFF_SHEAR_Y:	// shear up and down
		obj_aff_sheary(oaff, aff_value);
		break;
	default:			// shouldn't happen
		obj_aff_identity(oaff);
	}
}

void objaff_test()
{
	OBJ_ATTR *metr= &obj_buffer[0], *shadow= &obj_buffer[1];
	OBJ_AFFINE *oaff_curr= &obj_aff_buffer[0]; 
	OBJ_AFFINE *oaff_base= &obj_aff_buffer[1]; 
	OBJ_AFFINE *oaff_new=  &obj_aff_buffer[2];

	int x=96, y=32;
	int new_state;
	
	// oaff_curr = oaff_base * oaff_new
	// oaff_base changes when the aff-state changes
	// oaff_new is updated when it doesn't
	obj_aff_identity(oaff_curr);
	obj_aff_identity(oaff_base);
	obj_aff_identity(oaff_new);

	while(1)
	{
		key_poll();

		// move sprite around
		if( key_is_down(KEY_SELECT) && key_is_down(KEY_DIR) )
		{
			// move
			x += 2*key_tri_horz();
			y += 2*key_tri_vert();

			obj_set_pos(metr, x, y);
			obj_set_pos(shadow, x, y);
			new_state= AFF_NULL;
		}
		else	// or do an affine transformation
			new_state= get_aff_state();
		
		if(new_state != AFF_NULL)	// no change
		{
			if(new_state == aff_state)	// increase current transformation
			{
				get_aff_new(oaff_new);
				obj_aff_copy(oaff_curr, oaff_base, 1);
				obj_aff_postmul(oaff_curr, oaff_new);
			}
			else		// switch to different transformation type
			{
				obj_aff_copy(oaff_base, oaff_curr, 1);
				obj_aff_identity(oaff_new);
				aff_value= 0;
			}
			aff_state= new_state;
		}
		
		// START: toggles double-size flag
		// START+SELECT: resets obj_aff to identity
		if(key_hit(KEY_START))
		{
			if(key_is_down(KEY_SELECT))
			{
				obj_aff_identity(oaff_curr);
				obj_aff_identity(oaff_base);
				obj_aff_identity(oaff_new);
				aff_value= 0;
			}
			else
			{
				metr->attr0 ^= ATTR0_AFF_DBL_BIT;
				shadow->attr0 ^= ATTR0_AFF_DBL_BIT;
			}
		}
		
		vid_vsync();

		// we only have one OBJ_ATTR, so update that
		obj_copy(obj_mem, obj_buffer, 2);

		// we have 3 OBJ_AFFINEs, update these separately
		obj_aff_copy(obj_aff_mem, obj_aff_buffer, 3);

		// Display the current matrix
		tte_printf("#{es;P:8,136}P =  "
			"#{y:-7;Ps}| %04X\t%04X#{Pr;x:72}|"
			"#{Pr;y:12}| %04X\t%04X#{Pr;p:72,12}|", 
			(u16)oaff_curr->pa, (u16)oaff_curr->pb, 
			(u16)oaff_curr->pc, (u16)oaff_curr->pd);
	}
}

int main()
{
	REG_DISPCNT= DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
	oam_init(obj_buffer, 128);
	init_metr();

	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();
	tte_set_margins(8, 128, 232, 160);

	objaff_test();

	return 0;
}
