//
//  Link movement
//
//! \file link.c
//! \author J Vijn
//! \date 20070216 - 20070216
//
// === NOTES ===
// * Code is done for shortness and ease of use, 
//   not necessarily speed.

#include <tonc.h>

#include "link.h"

#define LINK_SPEED	0x0180

// --------------------------------------------------------------------
// EXTERNAL
// --------------------------------------------------------------------

typedef struct VIEWPORT
{
	int x, xmin, xmax, xpage;
	int y, ymin, ymax, ypage;
} VIEWPORT;

extern VIEWPORT g_vp; 
extern OBJ_ATTR obj_buffer[];

// --------------------------------------------------------------------
// DECLARATIONS
// --------------------------------------------------------------------

static void link_ani_stand(TSprite *link);
static void link_ani_walk(TSprite *link);

// --------------------------------------------------------------------
// LUTS
// --------------------------------------------------------------------

const u8 cLookDirs[4]= { 0, 1, 0, 2 };

const s8 cLinkShadow[4][2]= 
{	{ 1, 9 }, { 0, 8 }, { -1, 9 }, { 0, 8 }		};


const s8 cLinkStand[4]= { 32, 24, 32, 28};


// Movement luts
const s8 cLinkHeadYs[3][8]=
{
	{	-8, -9,  -9, -8, -8, -9,  -9, -8	},
	{	-8, -9, -10, -8, -8, -9, -10, -8	},
	{	-8, -9, -10, -8, -8, -9, -10, -8	}
};

const s8 cLinkHeadIds[3][8]= 
{
	{	4,  4, 16, 20,  4,  4, 16, 20	},
	{	8,  8,  8,  8,  8,  8,  8,  8	},
	{  12, 12, 12, 12, 12, 12, 12, 12	}
};
	
const s8 cLinkBodyIds[3][8]= 
{
	{	32,  36,  40,  44,  48,  52,  56,  60,	},
	{	64,  68,  72,  76,  64, -68, -72, -76,	},
	{	80,  84,  88,  92, -80, -84, -88, -92	}
};

// Default link attributes
const OBJ_ATTR cLinkObjs[3]= 
{
	{	0, ATTR1_SIZE_16,	 4, 0	}, 
	{	8, ATTR1_SIZE_16+1, 32, 0	},
	{	ATTR0_WIDE+17, ATTR1_SIZE_8+1, 2, 0	}
};


// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------

void link_init(TSprite *link, int x, int y, int obj_id)
{
	link->x= x;
	link->y= y;
	link->vx= link->vy= 0;

	link_set_state(link, SPR_STATE_STAND);
	link->dir= LOOK_RIGHT;
	link->objId= obj_id;

	oam_copy(&obj_buffer[obj_id], cLinkObjs, 3);
}

void link_set_state(TSprite *link, u32 state)
{
	link->state= state;
	link->aniFrame= 0;
}

void link_input(TSprite *link)
{
	link->vx= link->vy= 0;

	if( key_is_down(KEY_RIGHT) )
	{
		link->vx= LINK_SPEED;
		link->dir= LOOK_RIGHT;
	}
	else if( key_is_down(KEY_LEFT) )
	{
		link->vx= -LINK_SPEED;
		link->dir= LOOK_LEFT;
	}
	
	if( key_is_down(KEY_DOWN) )
	{
		link->vy= LINK_SPEED;
		link->dir= LOOK_DOWN;
	}
	else if( key_is_down(KEY_UP) )
	{
		link->vy= -LINK_SPEED;
		link->dir= LOOK_UP;
	}

	if( !key_is_down(KEY_DIR) )
		link_set_state(link, SPR_STATE_STAND);

	if( key_hit(KEY_DIR) )
		link_set_state(link, SPR_STATE_WALK);
}

void link_move(TSprite *link)
{
	// TODO : collision testing here?

	link->x += link->vx;
	link->y += link->vy;
}

void link_animate(TSprite *link)
{
	
	switch(link->state)
	{
	case SPR_STATE_STAND:
		link_ani_stand(link);
		break;
	case SPR_STATE_WALK:
		link->aniFrame += 0x56;
		link_ani_walk(link);
	}	
}

static void link_ani_stand(TSprite *link)
{
	POINT pt= { (link->x>>8)-g_vp.x, (link->y>>8) - g_vp.y };
	OBJ_ATTR *obj= &obj_buffer[link->objId];

	int dir= link->dir;
	int sublut= cLookDirs[dir];

	// Set body
	BFN_SET(obj[1].attr0, pt.y, ATTR0_Y);

	obj[1].attr1 &= ~(ATTR1_HFLIP | ATTR1_X_MASK);
	obj[1].attr1 |= BFN_PREP(pt.x, ATTR1_X);
	if(dir == LOOK_LEFT)
		obj[1].attr1 |= ATTR1_HFLIP;

	obj[1].attr2= cLinkStand[dir];

	// Set shadow
	obj_set_pos(&obj[2], 
		pt.x + cLinkShadow[dir][0], 
		pt.y + cLinkShadow[dir][1]);
		
	// Set head
	BFN_SET(obj[0].attr0, pt.y+cLinkHeadYs[sublut][0], ATTR0_Y);

	obj[0].attr1 &= ~(ATTR1_HFLIP | ATTR1_X_MASK);
	obj[0].attr1 |= BFN_PREP(pt.x-cLinkShadow[dir][0], ATTR1_X);
	
	if(dir == LOOK_LEFT)
		obj[0].attr1 |= ATTR1_HFLIP;

	obj[0].attr2= cLinkHeadIds[sublut][0];
}


static void link_ani_walk(TSprite *link)
{
	int dir= link->dir;
	int sublut= cLookDirs[dir];
	int frame= (link->aniFrame>>8) & 7;
	int tid;

	POINT pt= { (link->x>>8)-g_vp.x, (link->y>>8) - g_vp.y };
	OBJ_ATTR *obj= &obj_buffer[link->objId];


	// Set body
	BFN_SET(obj[1].attr0, pt.y, ATTR0_Y);

	obj[1].attr1 &= ~(ATTR1_HFLIP | ATTR1_X_MASK);
	obj[1].attr1 |= BFN_PREP(pt.x, ATTR1_X);
	if(dir == LOOK_LEFT)
		obj[1].attr1 |= ATTR1_HFLIP;

	tid= cLinkBodyIds[sublut][frame];
	if(tid<0)
	{
		obj[1].attr2= -tid;
		obj[1].attr1 ^= ATTR1_HFLIP;
	}
	else
		obj[1].attr2= tid;


	// Set shadow
	obj_set_pos(&obj[2], 
		pt.x + cLinkShadow[dir][0], 
		pt.y + cLinkShadow[dir][1]);
		
	// Set head
	BFN_SET(obj[0].attr0, pt.y+cLinkHeadYs[sublut][frame], ATTR0_Y);

	obj[0].attr1 &= ~(ATTR1_HFLIP | ATTR1_X_MASK);
	obj[0].attr1 |= BFN_PREP(pt.x-cLinkShadow[dir][0], ATTR1_X);
	
	if(dir == LOOK_LEFT)
		obj[0].attr1 |= ATTR1_HFLIP;

	obj[0].attr2= cLinkHeadIds[sublut][frame];
}

// EOF
