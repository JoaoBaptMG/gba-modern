//
//  Big map scrolling and sprite animation
//
//! \file bigmap.c
//! \author J Vijn
//! \date 20060508 - 20070216
//
// === NOTES ===


#include <stdio.h>
#include <string.h>
#include <tonc.h>

#include "link.h"

#include "kakariko.h"
#include "link_gfx.h"


#define kakarikoMapPitch 128

// === CONSTANTS ======================================================
// === CLASSES ========================================================


// TODO: rename structs and members

typedef struct VIEWPORT
{
	int x, xmin, xmax, xpage;
	int y, ymin, ymax, ypage;
} VIEWPORT;


typedef struct TMapInfo
{
	union
	{
		u32 state;			//!< Background state
		struct
		{
			u16 flags;
			u16 cnt;
		};
	};
	// Destination data
	SCR_ENTRY *dstMap;		//!< Screenblock pointer	
	// Source data
	SCR_ENTRY *srcMap;		//!< Source map address
	u32 srcMapWidth;		//!< Source map width
	u32 srcMapHeight;		//!< Source map height
	FIXED mapX;			//!< X-coord on map (.8f)
	FIXED mapY;			//!< Y-coord on map (.8f)
} TMapInfo;


// === GLOBALS ========================================================


VIEWPORT g_vp= 
{
	0, 0, 1024, 240, 
	0, 0, 1024, 160, 
};

TMapInfo g_bg;

OBJ_ATTR obj_buffer[128];

TSprite g_link;


// === PROTOTYPES =====================================================

INLINE void vp_center(VIEWPORT *vp, FIXED x, FIXED y);
void vp_set_pos(VIEWPORT *vp, FIXED x, FIXED y);

// === MACROS =========================================================
// === INLINES=========================================================
// === FUNCTIONS ======================================================


// --- VIEWPORT ---

INLINE void vp_center(VIEWPORT *vp, int x, int y)
{	vp_set_pos(vp, x - vp->xpage/2, y - vp->ypage/2);	}


void vp_set_pos(VIEWPORT *vp, int x, int y)
{
	vp->x= clamp(x, vp->xmin, vp->xmax - vp->xpage);
	vp->y= clamp(y, vp->ymin, vp->ymax - vp->ypage);
}


// --- TMapInfo ---

void bgt_init(TMapInfo *bgt, int bgnr, u32 ctrl, 
	const void *map, u32 map_width, u32 map_height)
{
	memset(bgt, 0, sizeof(TMapInfo));

	bgt->flags= bgnr;
	bgt->cnt= ctrl;
	bgt->dstMap= se_mem[BFN_GET(ctrl, BG_SBB)];

	REG_BGCNT[bgnr]= ctrl;
	REG_BG_OFS[bgnr].x= 0;
	REG_BG_OFS[bgnr].y= 0;


	bgt->srcMap= (SCR_ENTRY*)map;
	bgt->srcMapWidth= map_width;
	bgt->srcMapHeight= map_height;

	int ix, iy;
	SCR_ENTRY *dst= bgt->dstMap, *src= bgt->srcMap;
	for(iy=0; iy<32; iy++)
		for(ix=0; ix<32; ix++)
			dst[iy*32+ix]= src[	iy*bgt->srcMapWidth+ix];
}

void bgt_colcpy(TMapInfo *bgt, int tx, int ty)
{
	int iy, y0= ty&31;

	int srcP= bgt->srcMapWidth;	
	SCR_ENTRY *srcL= &bgt->srcMap[ty*srcP + tx];
	SCR_ENTRY *dstL= &bgt->dstMap[y0*32 + (tx&31)];

	for(iy=y0; iy<32; iy++)
	{	*dstL= *srcL;	dstL += 32;	srcL += srcP;	}

	dstL -= 1024;

	for(iy=0; iy<y0; iy++)
	{	*dstL= *srcL;	dstL += 32;	srcL += srcP;	}
}

void bgt_rowcpy(TMapInfo *bgt, int tx, int ty)
{
	int ix, x0= tx&31;

	int srcP= bgt->srcMapWidth;	
	SCR_ENTRY *srcL= &bgt->srcMap[ty*srcP + tx];
	SCR_ENTRY *dstL= &bgt->dstMap[(ty&31)*32 + x0];

	for(ix=x0; ix<32; ix++)
		*dstL++= *srcL++;

	dstL -= 32;

	for(ix=0; ix<x0; ix++)
		*dstL++= *srcL++;
}

void bgt_update(TMapInfo *bgt, VIEWPORT *vp)
{
	// Pixel coords
	int vx= vp->x, vy= vp->y;
	int bx= bgt->mapX, by= bgt->mapY;

	// Tile coords
	int tvx= vx>>3, tvy= vy>>3;
	int tbx= bx>>3, tby= by>>3;

	// Basically, we need another row/col when the viewport
	// exposes another row/col, i.e., if the tile coords
	// have changed

	if(tvx < tbx)		// add on left
		bgt_colcpy(bgt, tvx, tvy);
	else if(tvx > tbx)	// add on right
		bgt_colcpy(bgt, tvx+31, tvy);

	if(tvy < tby)		// add on top
		bgt_rowcpy(bgt, tvx, tvy);
	else if(tvy > tby)	// add on bottom
		bgt_rowcpy(bgt, tvx, tvy+31);

	// Update TMapInfo and reg-offsets
	int bgnr= bgt->flags;
	REG_BG_OFS[bgnr].x= bgt->mapX= vx;
	REG_BG_OFS[bgnr].y= bgt->mapY= vy;
}

void init_textbox(int bgnr, int left, int top, int right, int bottom)
{	
	tte_set_margins(left, top, right, bottom);

	REG_DISPCNT |= DCNT_WIN0;

	REG_WIN0H= left<<8 | right;
	REG_WIN0V= top<<8 | bottom;
	REG_WIN0CNT= WIN_ALL | WIN_BLD;
	REG_WINOUTCNT= WIN_ALL;

	REG_BLDCNT= (BLD_ALL&~BIT(bgnr)) | BLD_BLACK;
	REG_BLDY= 5;
}

void init_main()
{
	// Basic setups
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	oam_init(obj_buffer, 128);

	// Bigmap setup
	LZ77UnCompVram(kakarikoTiles, tile_mem[0]);
	GRIT_CPY(pal_bg_mem, kakarikoPal);

	bgt_init(&g_bg, 1, BG_CBB(0)|BG_SBB(29), kakarikoMap, 
		128, 128);

	// Object setup
	GRIT_CPY(pal_obj_mem, link_gfxPal);
	GRIT_CPY(tile_mem[4], link_gfxTiles);

	link_init(&g_link, 120<<8, 80<<8, 0);

	//# NOTE: erasing and rendering text flows over into the VDRAW period.
	//# Using the ASM renderer and placing the text at the bottom limits its effects.
	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_set_drawg(chr4c_drawg_b4cts_fast);
	tte_init_con();

	//init_textbox(0, 8, 8, SCR_W-8, 8+2*12);

	init_textbox(0, 8, SCR_H-(8+2*12), SCR_W-8, SCR_H-8);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | 
		DCNT_OBJ_1D | DCNT_WIN0;
}

int main()
{
	init_main();
	
	int x, y;

	while(1)
	{
		VBlankIntrWait();

		key_poll();

		link_input(&g_link);
		link_animate(&g_link);
		link_move(&g_link);

		x= g_link.x>>8, y= g_link.y>>8;

		vp_center(&g_vp, x, y);
		oam_copy(oam_mem, obj_buffer, 128);

		bgt_update(&g_bg, &g_vp);

		tte_printf("#{es;P}( x, y) = (%d,%d)\n(vx,vy) = (%d,%d)", 
			x, y, g_vp.x, g_vp.y);
		pal_bg_mem[0]= REG_VCOUNT;
	}

	return 0;
}

// EOF
