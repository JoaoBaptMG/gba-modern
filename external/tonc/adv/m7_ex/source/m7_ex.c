//
//  [[ ]]
//
//! \file [[ ]]
//! \author J Vijn
//! \date 20061108 - 20080322
//
// === NOTES ===

#include <limits.h>
#include <stdio.h>

#include <tonc.h>
#include "mode7.h"

#include "all_gfx.h"


#define SPR_COUNT	24

#define MENU_CHR	128
#define MENU_TEXT_SE0	0xF001
#define MENU_FRAME_SE0	0xF080
#define MENU_X0		56
#define MENU_Y0		40

enum eMenuOpt
{	MENU_MOVE=0, MENU_FOG, MENU_OBJ_VIEW, MENU_RESET, MENU_EXIT	};

#define STATE_MOVE_LOCAL	(0<<0)
#define STATE_MOVE_LEVEL	(1<<0)	
#define STATE_MOVE_GLOBAL	(2<<0)
#define STATE_FOG			(1<<2)
#define STATE_OBJ_VIEW_ATAN	(1<<3)
#define STATE_MENU			(1<<31)

#define STATE_MOVE_SHIFT	0
#define STATE_MOVE_MASK		(3<<0)

// === GLOBALS ========================================================

M7_CAM m7_cam;
BG_AFFINE m7_bgaffs[SCREEN_HEIGHT+1];
M7_SPRITE m7_sprites[SPR_COUNT];

M7_LEVEL m7_level;

u32 g_state= STATE_MOVE_LEVEL | STATE_FOG;
u32 g_menu_sel= MENU_EXIT;

u8 sort_ids[SPR_COUNT];
int sort_keys[SPR_COUNT];

// --- constants (TODO: move elsewhere) ---

const M7_CAM m7_cam_default= 
{
	{ 0x0D100, 0x1900, 0x38800 }, 
	0x0A00, 0x2600, 
	{ 256, 0, 0 }, {0, 256, 0}, {0, 0, 256}
};

// 2 sets of pobj positions/angles
// One for starting grid, one for action positions

#if 0

const VECTOR cKartPos[8]= 
{
	{ 0x39000, 0x0, 0x2A800 }, 	// Mario
	{ 0x3B000, 0x0, 0x2C000 }, 	// Luigi
	{ 0x39000, 0x0, 0x2D800 }, 	// Princess
	{ 0x3B000, 0x0, 0x2F000 }, 	// Yoshi
	{ 0x39000, 0x0, 0x30800 }, 	// Bowser
	{ 0x3B000, 0x0, 0x32000 }, 	// Donkey
	{ 0x39000, 0x0, 0x33800 }, 	// Koopa
	{ 0x3B000, 0x0, 0x35000 } 	// Toad
};

const s16 cKartPhi[8]= 
{
	0, 0, 0, 0, 0, 0, 0, 0
};

#else

const VECTOR cKartPos[8]= 
{
	{ 0x21C00, 0x0, 0x14000 }, 	// Mario
	{ 0x1C600, 0x0, 0x20800 }, 	// Luigi
	{ 0x3A000, 0x0, 0x1E800 }, 	// Princess
	{ 0x1D000, 0x0, 0x20000 }, 	// Yoshi
	{ 0x0F000, 0x0, 0x36000 }, 	// Bowser
	{ 0x39000, 0x0, 0x2A800 }, 	// Donkey
	{ 0x11800, 0x0, 0x36800 }, 	// Koopa
	{ 0x10000, 0x0, 0x35800 } 	// Toad
};

const s16 cKartPhi[8]= 
{
	0x4000, 0x1000, 0, 0, 0x7000, 0, 0x4200, 0x5600
};

#endif

const u16 cKartAttr2[8]= 
{	
	0x1420, 0x2430, 0x3440, 0x0450, 
	0x0460, 0x3470, 0x2480, 0x1490 
};

const u8 cKartFrames[32]= 
{
	 0,  1,  2, 3, 4, 5, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 
	11, 10, 10, 9, 9, 8, 8, 7, 7, 6, 5, 4, 3,  2,  1,  0, 
};

const VECTOR cThwompPos[16]=
{
	{ 0x18400, 0x1000, 0x02000 }, 
	{ 0x18400, 0x0C00, 0x03000 }, 
	{ 0x18400, 0x0800, 0x04000 }, 
	{ 0x18400, 0x0400, 0x05000 }, 
	{ 0x0A000, 0x1000, 0x35C00 }, 
	{ 0x0B000, 0x1000, 0x39C00 }, 
	{ 0x0D800, 0x1000, 0x37400 }, 
	{ 0x0F800, 0x0400, 0x35400 }, 
	{ 0x1B800, 0x0800, 0x1FC00 }, 
	{ 0x1C800, 0x0200, 0x1FC00 }, 
	{ 0x1D800, 0x0200, 0x1FC00 }, 
	{ 0x1E800, 0x1000, 0x1FC00 }, 
	{ 0x2AC00, 0x1000, 0x3A000 }, 
	{ 0x2B400, 0x0400, 0x3C800 }, 
	{ 0x2D000, 0x1000, 0x38800 }, 
	{ 0x2D000, 0x1000, 0x3B800 }

};


// === PROTOTYPES =====================================================

IWRAM_CODE void id_sort_shell(int keys[], u8 ids[], int count);

void kart_animate(M7_SPRITE *spr, const M7_CAM *cam);

void input();
void input_game();
void input_menu();

void init_main();
void init_menu();
void init_sprites();

void update_sprites();

// === FUNCTIONS ======================================================

// --- kart functions ---

//! Animate kart sprite
void kart_animate(M7_SPRITE *spr, const M7_CAM *cam)
{
	OBJ_ATTR *obj= &spr->obj;

	if(BFN_GET2(obj->attr0,ATTR0_MODE) == ATTR0_HIDE)
		return;
	 
	// NOTE 1: Yes, I really do need the s16 for psi here.
	// NOTE 2: or not, if I used arctan for the angle, like I 
	//   probably should.
	TILE *dst= &tile_mem[4][BFN_GET(obj->attr2, ATTR2_ID)];
	s16 psi= spr->phi - cam->phi;
	
	// Extra arctan angle for correctness
	if(g_state & STATE_OBJ_VIEW_ATAN) 
		psi -= ArcTan2(spr->pos2.z>>8, spr->pos2.x>>8);

	memcpy32(dst, &spr->tiles[cKartFrames[(psi>>11)&31]*16], 16*8);

	OBJ_AFFINE *oa= &obj_aff_mem[spr->aff_id];
	if(psi < 0)
		oa->pa= -oa->pa;
}

void update_sprites()
{
	int ii;

	M7_SPRITE *spr= m7_level.sprites;
	for(ii=0; ii<SPR_COUNT; ii++)
	{
		m7_prep_sprite(&m7_level, &spr[ii]);

		// Create sort key
		if(BFN_GET2(spr[ii].obj.attr0, ATTR0_MODE) != ATTR0_HIDE)
			sort_keys[ii]= spr[ii].pos2.z;
		else
			sort_keys[ii]= INT_MAX;
	}

	id_sort_shell(sort_keys, sort_ids, SPR_COUNT);

	// Animate karts
	for(ii=0; ii<8; ii++)
		kart_animate(&spr[ii], m7_level.camera);

	// Update real OAM
	for(ii=0; ii<SPR_COUNT; ii++)
		obj_copy(&oam_mem[ii], &spr[sort_ids[ii]].obj, 1);
}


// --- Initialisers ---------------------------------------------------

void init_main()
{
	// Init mode 7
	m7_init(&m7_level, &m7_cam, m7_bgaffs, m7_sprites,  	
		BG_CBB(0) | BG_SBB(22) | BG_REG_64x32 | BG_PRIO(3),
		BG_CBB(0) | BG_SBB(24) | BG_AFF_128x128 | BG_WRAP | BG_PRIO(3));
	*m7_level.camera = m7_cam_default;

	// Setup main bg
	LZ77UnCompVram(bgPal, pal_bg_mem);
	LZ77UnCompVram(bc1floorTiles, tile_mem[0]);
	LZ77UnCompVram(bc1floorMap, se_mem[24]);

	// For fade to orange
	REG_BLDCNT= BLD_BUILD(BLD_BG2, BLD_BACKDROP, 1);
	pal_bg_mem[0]= CLR_ORANGE;

	// Setup sky bg
	LZ77UnCompVram(bc1skyTiles, &tile_mem[0][128]);
	LZ77UnCompVram(bc1skyMap, se_mem[22]);

	// Setup sprites
	oam_init(oam_mem, 128);
	LZ77UnCompVram(objPal, pal_obj_mem);
	LZ77UnCompVram(thwompTiles, &tile_mem[4][16]);
	init_sprites();

	// Setup menu
	init_menu();

	// Registers
	REG_DISPCNT= DCNT_MODE1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

	irq_init(NULL);
	irq_add(II_HBLANK, (fnptr)m7_hbl_floor);
	irq_add(II_VBLANK, NULL);
}

void init_sprites()
{
	int ii;
	M7_SPRITE *spr= m7_level.sprites;

	// Karts:
	for(ii=0; ii<8; ii++)
	{
		spr->pos= cKartPos[ii];
		spr->anchor.x= 16;
		spr->anchor.y= 30;
		obj_set_attr(&spr->obj, ATTR0_SQUARE | ATTR0_AFF_DBL, 
			ATTR1_SIZE_32 | ATTR1_AFF_ID(ii), cKartAttr2[ii]);
		spr->phi= cKartPhi[ii];
		spr->obj_id= ii;
		spr->aff_id= ii;
		spr->tiles= &((TILE*)kartsTiles)[ii*16*12];

		spr++;
	}

	// Thwomps
	for(ii=0; ii<16; ii++)
	{
		spr->pos= cThwompPos[ii];
		spr->anchor.x= 16;
		spr->anchor.y= 30;
		obj_set_attr(&spr->obj, ATTR0_SQUARE | ATTR0_AFF_DBL, 
			ATTR1_SIZE_32 | ATTR1_AFF_ID(ii+8), 0x4410);
		spr->phi= 0;
		spr->obj_id= ii+8;
		spr->aff_id= ii+8;
		spr->tiles= NULL;

		spr++;
	}

	// Setup sorting list
	for(ii=0; ii<SPR_COUNT; ii++)
		sort_ids[ii]= ii;
}

void init_menu()
{
	int ii;

	tte_init_se(0, BG_CBB(1) | BG_SBB(7), MENU_TEXT_SE0, CLR_BLUE, 0xEE, 
		NULL, NULL);
	tte_init_con();

	u32 *pwd= (u32*)&tile_mem[1][1];
	for(ii=0; ii<96*8; ii++)
		*pwd++ |= octup(8);

	LZ77UnCompVram(borderTiles, &tile_mem[1][128]);
	GRIT_CPY(pal_bg_bank[15], borderPal);

	se_window(se_mem[7], 6, 4, 24, 11, MENU_FRAME_SE0);
	tte_set_margins(MENU_X0+8, MENU_Y0, 184, 80);
	tte_set_paper(MENU_TEXT_SE0);
	tte_set_special(MENU_TEXT_SE0);
	tte_write(
		"#{es;P}motion : level \n"
		"fog    : on \n"
		"anim   : base\n"
		"reset\n"
		"continue");
}


// --- Input ----------------------------------------------------------

void input()
{
	key_poll();

	if(g_state & STATE_MENU)
		input_menu();
	else
		input_game();
}

void input_game()
{
	const FIXED VEL_H= 0x200, VEL_Y= 0x80, OMEGA= 0x140;
	VECTOR dir= {0, 0, 0};
	
	dir.x= VEL_H*key_tri_shoulder();	// strafe

	if(key_is_down(KEY_SELECT))
		dir.y= VEL_Y*key_tri_fire();	// B/A : sink/rise
	else
		dir.z= -VEL_H*key_tri_fire();	// B/A : back/forward

	M7_CAM *cam= m7_level.camera;

	// Change camera orientation
	m7_rotate(cam, 
		cam->phi   + OMEGA*key_tri_horz(),	// look left/right
		cam->theta - OMEGA*key_tri_vert());	// look up.down

	// and position
	switch(BFN_GET2(g_state, STATE_MOVE))
	{
	case STATE_MOVE_LOCAL:
		m7_translate_local(cam, &dir);
		break;
	case STATE_MOVE_LEVEL:
		m7_translate_level(cam, &dir);
		break;
	case STATE_MOVE_GLOBAL:
		m7_translate_global(cam, &dir);
		break;
	}

	// Don't sink through the ground
	if(cam->pos.y < 2<<8)
		cam->pos.y= 2<<8;
		
	if(key_hit(KEY_START))
	{
		REG_DISPCNT |= DCNT_BG0;
		g_state |= STATE_MENU;
	}	
}

void input_menu()
{
	extern u8 *__text_start;

	u32 sel= g_menu_sel, opt;
	const char *const move_str[3]= {"local ", "level ", "global"};

	if(key_hit(KEY_START|KEY_B))
	{
		REG_DISPCNT &= ~DCNT_BG0;
		g_state &= ~STATE_MENU;
		return;
	}

	tte_printf("#{P:%d,%d} ", MENU_X0, MENU_Y0+sel*8);

	if(key_hit(KEY_SELECT|KEY_DOWN))
		sel++;
	else if(key_hit(KEY_UP))
		sel--;
	else if(key_hit(KEY_LEFT|KEY_RIGHT|KEY_A))
	{
		switch(sel)
		{
		case MENU_MOVE:
			opt= BFN_GET(g_state, STATE_MOVE);
			if(key_hit(KEY_RIGHT|KEY_A))
				opt++;
			else
				opt--;
			opt= wrap(opt, 0, 3);
			BFN_SET(g_state, opt, STATE_MOVE);
			tte_set_pos(MENU_X0+80, MENU_Y0);
			tte_write(move_str[opt]);
			break;

		case MENU_FOG:
			g_state ^= STATE_FOG;

			tte_set_pos(MENU_X0+80, MENU_Y0+8);
			tte_write(g_state&STATE_FOG ? "on ": "off");
			REG_BLDCNT ^= BLD_BG2;
			break;

		case MENU_OBJ_VIEW:
			g_state ^= STATE_OBJ_VIEW_ATAN;
			tte_set_pos(MENU_X0+80, MENU_Y0+16);
			tte_write(g_state&STATE_OBJ_VIEW_ATAN ? "base": "atan");
			break;

		case MENU_RESET:
			// REG_RESET_DST needs to be set to 1 for multiboot games
			// __text_start is a DKP variable indicating the start of 
			// code, so if this is in EWRAM we know it's MB
			// Should probably go in a function though

			// NOTE: this may not always work >_>
			if( (u32)__text_start & 0x02000000)
				REG_RESET_DST= 1;
			SoftReset();
			break;

		case MENU_EXIT:
			REG_DISPCNT &= ~DCNT_BG0;
			g_state &= ~STATE_MENU;
			break;
		}
	}
	sel= wrap(sel, 0, 5);

	tte_printf("#{P:%d,%d}>", MENU_X0, MENU_Y0+sel*8);

	g_menu_sel= sel;
}

// --- Main -----------------------------------------------------------

int main()
{
	init_main();

	while(1)
	{
		VBlankIntrWait();
		input();

		m7_prep_horizon(&m7_level);
		if(m7_level.horizon > 0)
		{
			BFN_SET(REG_DISPCNT, DCNT_MODE0, DCNT_MODE);
			REG_BG2CNT= m7_level.bgcnt_sky;
			REG_BLDALPHA= 16;	
		}
		m7_update_sky(&m7_level);

		update_sprites();
		m7_prep_affines(&m7_level);

		// scanline ~ [172 - 213]
	}

	return 0;

}

// EOF
