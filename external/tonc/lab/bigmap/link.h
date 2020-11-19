//
//  Sprite Link movement
//
//! \file link.h
//! \author J Vijn
//! \date 20070216 - 20070216
//
// === NOTES ===


#ifndef __LINK__
#define __LINK__


// === CONSTANTS ======================================================

enum ELookDir
{	
	LOOK_RIGHT= 0, LOOK_DOWN, LOOK_LEFT, LOOK_UP, 
};

#define SPR_STATE_STAND		0x0100
#define SPR_STATE_WALK		0x0200

// === CLASSES ========================================================

typedef struct TSprite
{
	FIXED		x, y;		//!< Position
	FIXED		vx, vy;		//!< Velocity
	u16			state;		//!< Sprite state
	u8			dir;		//!< Look direction
	u8			objId;		//!< Object index
	FIXED		aniFrame;	//!< Animation frame counter
} TSprite;

#endif // __LINK__


// -------------------------------------------------------------------- 
// PROTOTYPES
// -------------------------------------------------------------------- 

void link_init(TSprite *link, int x, int y, int obj_id);
void link_set_state(TSprite *link, u32 state);
void link_input(TSprite *link);

void link_move(TSprite *link);
void link_animate(TSprite *link);



// EOF
