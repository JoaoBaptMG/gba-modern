//
// win_demo.c
// Playing hide and seek with a rocket item
// using windowing
//
// (20040104 - 20060926, cearn)
// === NOTES ===
//   bg 0, cbb  0, sbb  2, pb 0: numbered forground
//   bg 1, cbb  0, sbb  3, pb 0: fenced background
//   oam 0: tile 0-3: rocket

//   win 0: objects
//   win 1: bg 0
//   win out : bg 1

#include <tonc.h>
#include "nums.h"
#include "rocket.h"

#define ROCKET_OID	0


// --- global buffers for window registers ---
// This will allow you to poke them inside IWRAM, 
// since you can't poke REG_WINx directly. :(

typedef struct tagRECT_U8 { u8 ll, tt, rr, bb; } RECT_U8;

// Objects in win0, BG in win1
RECT_U8 win[2]= 
{
	{ 36, 20,  76,  60 },	// win0: 40x40 rect
	{ 12, 12 ,228, 148 }	// win1: screen minus 12 margin.
};

// foreground map is alternating numbered tile pattern
// is inside window 1
void init_front_map()
{
	int ii, jj;

	memcpy32(&tile8_mem[0][1], nums8Tiles, nums8TilesLen/4);
	memcpy32(pal_bg_mem, numsPal, numsPalLen/4);
	pal_bg_mem[0]= CLR_BLACK;

	u16 *pse= &se_mem[2][0];
	for(ii=0; ii<20; ii++)
		for(jj=0; jj<32; jj++)
			*pse++= ((ii+jj)&0x0F)+1;

	REG_BG0CNT= BG_CBB(0) | BG_SBB(2) | BG_8BPP;
}

// fill a second background with a (not so) nice thread pattern
// is on outside
void init_back_map()
{
	const TILE back_tile=
	{{
		0x33123333, 0x33123333, 0x13213331, 0x21212112,
		0x12121221, 0x33123332, 0x33213333, 0x33213333
	}};

	tile_mem[0][0x30]= back_tile;
	
	pal_bg_mem[0x21]= 0x09BA;
	pal_bg_mem[0x22]= 0xC0DE;
	pal_bg_mem[0x23]= 0x0015;
	pal_bg_mem[0x24]= 0xC001;

	u32 ses= 0x30 | SE_PALBANK(2);	// tile 0x30, pal-bank 2
	ses |= (ses | SE_HFLIP | SE_VFLIP)<<16;

	memset32(&se_mem[3][0], ses, 32*20/2);

	REG_BG1CNT= BG_CBB(0) | BG_SBB(3);
}

// rocket is inside window 0
void init_rocket()
{
	memcpy32(&tile_mem[4][0], rocketTiles, rocketTilesLen/4);
	memcpy32(pal_obj_mem, rocketPal, rocketPalLen/4);

	OBJ_ATTR *rocket= &oam_mem[ROCKET_OID];
	obj_set_attr(rocket, ATTR0_SQUARE, ATTR1_SIZE_16, 0);
	obj_set_pos(rocket, 96, 32);
}

void win_copy()
{
	REG_WIN0H= win[0].ll<<8 | win[0].rr;
	REG_WIN1H= win[1].ll<<8 | win[1].rr;
	REG_WIN0V= win[0].tt<<8 | win[0].bb;
	REG_WIN1V= win[1].tt<<8 | win[1].bb;
}

void test_win()
{
	win_copy();
	while(1)
	{
		key_poll();
		vid_vsync();

		// key_hit() or key_is_down() 'switch'
		// A depressed: move on direction press (std movement)
		// A pressed  : moves on direction hit (fine movement)
		int keys= key_curr_state();
		if(key_is_down(KEY_A))
			keys &= ~key_prev_state();

		if(keys & KEY_RIGHT)
		{	win[0].ll++;		win[0].rr++;	}
		else if(keys & KEY_LEFT )
		{	win[0].ll--;		win[0].rr--;	}
		if(keys & KEY_DOWN)
		{	win[0].tt++;		win[0].bb++;	}
		else if(keys & KEY_UP )
		{	win[0].tt--;		win[0].bb--;	}

		// randomize rocket position
		if(key_hit(KEY_START))
			obj_set_pos(&oam_mem[ROCKET_OID], 
				qran_range(0, 232), qran_range(0, 152));

		win_copy();
	}
}

int main()
{
	// obvious inits
	oam_init(oam_mem, 128);
	init_front_map();
	init_back_map();
	init_rocket();
	win_copy();

	// windowing inits
	//win_init(DCNT_WIN0 | DCNT_WIN1);
	REG_DISPCNT= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | 
		DCNT_OBJ_1D | DCNT_WIN0 | DCNT_WIN1;

	REG_WININ= WININ_BUILD(WIN_OBJ, WIN_BG0);
	REG_WINOUT= WINOUT_BUILD(WIN_BG1, 0);

	test_win();

	return 0;
}
