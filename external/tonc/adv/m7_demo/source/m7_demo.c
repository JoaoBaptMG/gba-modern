//
// m7_demo.c
// block, sawtooth and smooth mode7 in one demo :)
// Using pre-calculated LUTs, courtesy of my excellut program
// 
//
// Coordinate system:
//
//    y
//     |
//     |___x
//  z /
//
// The viewing direction is in the NEGATIVE z direction!!
//
// (20040104 - 20070808, Cearn)

#include <stdio.h>
#include <tonc.h>

#include "m7_demo.h"
#include "nums.h"

// === CONSTANTS & MACROS =============================================

#define MAP_AFF_SIZE 0x0100
#define PHI0  0

static const VECTOR cam_pos_default= { 256<<8, 32<<8, 256<<8 };

CSTR strings[]= 
{
	"A: blocky", 
	"B: sawtooth", 
	"C: smooth"
};

// === GLOBALS ========================================================

VECTOR cam_pos;
u16 cam_phi= PHI0;

FIXED g_cosf= 1<<8, g_sinf= 0;	// temporaries for cos and sin cam_phi
int g_state= M7_BLOCK;


fnptr m7_isrs[3]= 
{
	(fnptr)m7_hbl_a,
	(fnptr)m7_hbl_b,
	(fnptr)m7_hbl_c,
};

// === FUNCTIONS ======================================================

void init_main()
{
	int ii;

	cam_pos= cam_pos_default;

	GRIT_CPY(&tile8_mem[0][1], numsTiles);
	GRIT_CPY(pal_bg_mem, numsPal);
		
	// Fill the map with a band pattern
	for(ii=0; ii<16; ii++)
		memset32(&se_mem[8][ii*16*16/2], quad8(ii+1), 16*16/4);

	REG_BG2CNT= BG_CBB(0) | BG_SBB(8) | BG_AFF_64x64;
}

void input_main()
{
	const FIXED speed= 2, DY= 64;
	VECTOR dir;

	key_poll();

	dir.x= speed*key_tri_horz();	// left/right : strafe
	dir.y= DY* key_tri_fire();		// B/A : rise/sink
	dir.z= speed*key_tri_vert();	// up/down : forward/back

	cam_pos.x += dir.x*g_cosf - dir.z*g_sinf;
	cam_pos.y += dir.y;
	cam_pos.z += dir.x*g_sinf + dir.z*g_cosf;

	if(cam_pos.y<0)
		cam_pos.y= 0;

	cam_phi += 128*key_tri_shoulder();

	if(key_hit(KEY_START))
	{
		cam_pos= cam_pos_default;
		cam_phi= PHI0;
		set_mode7_type(M7_BLOCK);
	}

	if(key_hit(KEY_SELECT))
		set_mode7_type(g_state+1);

	g_cosf= lu_cos(cam_phi)>>4;
	g_sinf= lu_sin(cam_phi)>>4;
}

int main()
{
	init_main();

	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();
	tte_set_margins(8, 8, 232, 40);

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_HBLANK, NULL);
	// and vblank int for vsync
	irq_add(II_VBLANK, NULL);
	set_mode7_type(M7_BLOCK);

	REG_DISPCNT= DCNT_MODE1 | DCNT_BG0 | DCNT_BG2;

	// main loop
	while(1)
	{
		VBlankIntrWait();
		input_main();
	}
	return 0;
}

// === MODE7 TYPES ====================================================

void set_mode7_type(int type)
{
	type= wrap(type, M7_BLOCK, M7_SMOOTH+1); 

	irq_add(II_HBLANK, m7_isrs[type]);

	g_state= type;

	tte_printf("#{es;P}%s", strings[g_state]);
}
