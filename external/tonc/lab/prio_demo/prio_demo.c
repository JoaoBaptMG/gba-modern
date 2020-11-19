//
// prio_demo.c
// Fun with priorities
//
// (20060223 - 20070809, cearn)
// === NOTES ===
// * Almost done ... fixing controls

#include <stdio.h>

#include <tonc.h>

#include "prio_gfx.h"
#include "prio_bg.h"

// --------------------------------------------------------------------
//  TYPES
// --------------------------------------------------------------------


typedef struct TSprite
{
	VECTOR pos;	// position
	int id;		// oe-id in OAM buffer
} TSprite;


// --------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------


#define SPR_COUNT 96

const VECTOR P_ORG= { 112<<8, 0<<8, 0<<8 };
#define AMP	    0x3800  // amplitude (.8)
#define WAVEN  -0x002C  // wave number (.12)
#define OMEGA0  0x0200  // angular velocity (.8)

#define S_AUTO      0x0001
#define S_SORT      0x0002


// --------------------------------------------------------------------
// GLOBALS
// --------------------------------------------------------------------


TSprite sprites[SPR_COUNT];		// Sprite list
u8 sort_ids[SPR_COUNT];			// Sorted OAM indices
int sort_keys[SPR_COUNT];		// sort keys
int g_phi= 0;					// phase, integration of omega over time
int g_omega= OMEGA0;			// rotation velocity (.8)
u32 g_state= S_AUTO | S_SORT;

int g_time;	// profiling

OBJ_ATTR oam_buffer[SPR_COUNT];


// --------------------------------------------------------------------
// PROTOTYPES
// --------------------------------------------------------------------


void init_main();

void spr_helix(const VECTOR *p0, int phi0);
void spr_update();

IWRAM_CODE void id_sort_shell(int keys[], u8 ids[], int count);
IWRAM_CODE void id_sort_shell2(int keys[], u8 ids[], int count);


// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------


// Helical pattern:
//   ft(t) = INT(w(t'), t', 0, t)
//   (x,y,z) = ( x0+A*cos(k*y+ft), y, A*sin(k*y+ft) )
void spr_helix(const VECTOR *p0, int phi0)
{
	int ii, phi;
	VECTOR dp= {0, 0, 0};
	TSprite *sprL= sprites, *sprR= &sprites[SPR_COUNT/2];

	for(ii=0; ii<SPR_COUNT/2; ii++)
	{
		// phi/phi0: 0.16f ; dp: .8f ; WAVEN:0.12f
		phi= (WAVEN*dp.y>>4) + (phi0);

		// red helix
		dp.x=  AMP*lu_cos(phi)>>12;		// .8f
		dp.z=  AMP*lu_sin(phi)>>12;		// .8f
		vec_add(&sprL[ii].pos, p0, &dp);

		// cyan helix
		dp.x= -dp.x;
		dp.z= -dp.z;
		vec_add(&sprR[ii].pos, p0, &dp);

		dp.y += 144*256/(SPR_COUNT/2);
	}
}

void spr_update()
{
	int ii;
	int prio, zz;
	u32 attr2;
	int *key= sort_keys;
	TSprite *spr= sprites;
	OBJ_ATTR *obj;

	for(ii=0; ii<SPR_COUNT; ii++)
	{
		obj= &oam_buffer[spr->id];
		// set x/y pos
		obj_set_pos(obj, spr->pos.x>>8, spr->pos.y>>8);

		// set priority based on depth.
		attr2= obj->attr2 & ~(ATTR2_PRIO_MASK | ATTR2_PALBANK(1));
		zz= spr->pos.z;
		if(zz>0)
		{
			prio= 2;
			attr2 |= ATTR2_PALBANK(1);
		}
		else
			prio= 1;
		obj->attr2= attr2 | ATTR2_PRIO(prio);
		
		// set sort key
		// key{1E-1F} : priority
		// key{00-1D} : depth value = -zz
		// Sort gives an ascending array, but key-signs mean the 
		// following ranges
		//   prio 0 : [E, 2> *1000:0000 = [-2,+2>
		//   prio 3 : [A, E> *1000:0000 = [-6,-2>
		// to make prio 0 be lowest, subtract 6 (=8-E)
		*key++= (prio<<30) + (zz>>2) - (6<<28);	

		/* // If you want to exclude hidden sprites, use this:
		if( (oe->attr0&OE_A0_MODE_MASK) != OE_A0_HIDE )
			sort_keys[ii]= (prio<<30) + (zz>>2) - (6<<28);	
		else
			sort_keys[ii]= (1<<31)-1;
		*/
		spr++;
	}

	profile_start();
	if(g_state & S_SORT)
	{
		// sort
		id_sort_shell(sort_keys, sort_ids, SPR_COUNT);
		// try the asm version too some time
		//id_sort_shell2(sort_keys, sort_ids, SPR_COUNT);

		// update
		for(ii=0; ii<SPR_COUNT; ii++)
			oam_mem[ii]= oam_buffer[sort_ids[ii]];
	}
	else
		oam_copy(oam_mem, oam_buffer, SPR_COUNT);
	g_time= profile_stop();
}


void init_main()
{
	int ii;

	// --- init gfx ---
	// bgs
	memcpy32(pal_bg_mem, cagePal, cagePalLen/4);
	pal_bg_mem[0]= CLR_BLACK;
	memcpy32(tile_mem[1], cageTiles, cageTilesLen/4);

	// there are 3 maps in cageMap, which have to be extracted manually
	// ("ZOMG, HAX!" Yeah. So?)
	// front part, priority 0
	memcpy32(se_mem[5], &cageMap[ 1*32], 20*32/2);
	REG_BG1CNT= BG_CBB(1) | BG_SBB(5) | BG_8BPP | BG_PRIO(0);
	// center, priority 1
	memcpy32(se_mem[6], &cageMap[22*32], 20*32/2);
	REG_BG2CNT= BG_CBB(1) | BG_SBB(6) | BG_8BPP | BG_PRIO(1);
	// back, priority 2
	memcpy32(se_mem[7], &cageMap[43*32], 20*32/2);
	REG_BG3CNT= BG_CBB(1) | BG_SBB(7) | BG_8BPP | BG_PRIO(2);

	// object
	memcpy32(&tile_mem[4][1], ballTiles, ballTilesLen/4);
	memcpy32(pal_obj_mem, ballPal, ballPalLen/4);

	// -- init vars ---
	// init sort list
	for(ii=0; ii<SPR_COUNT; ii++)
		sprites[ii].id= sort_ids[ii]= ii;

	// --- init sprites and objects ---
	oam_init(oam_buffer, SPR_COUNT);
	for(ii=0; ii<SPR_COUNT/2; ii++)
	{
		obj_set_attr(&oam_buffer[ii], 0, ATTR1_SIZE_16, 0x4001);
		obj_set_attr(&oam_buffer[ii+SPR_COUNT/2], 0, 
			ATTR1_SIZE_16, 0x6001);
	}

	spr_helix(&P_ORG, 0);
	spr_update();

	REG_DISPCNT= DCNT_MODE0 | DCNT_LAYER_MASK | DCNT_OBJ_1D;

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_init_con();
	tte_set_margins(8, 132, 232, 160);
	pal_bg_bank[15][1]= CLR_RED;
	pal_bg_bank[15][2]= CLR_MAROON;
}

int main()
{
	init_main();

	while(1)
	{
		VBlankIntrWait();
		key_poll();
		if(key_hit(KEY_START))
			g_state ^= S_AUTO;
		if(key_hit(KEY_SELECT))
			g_state ^= S_SORT;

		if(g_state & S_AUTO)
		{
			g_omega += key_tri_shoulder()<<4;
			g_phi += g_omega;
		}
		else
			g_phi += g_omega*key_tri_shoulder();

		spr_helix(&P_ORG, g_phi);
		spr_update();

		tte_printf("#{es;P}%6d", g_omega);
	}

	return 0;
}
