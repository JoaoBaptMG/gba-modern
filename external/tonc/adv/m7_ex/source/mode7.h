//
//  Mode 7 header file
//
//! \file mode7.c
//! \author J Vijn
//! \date 20061111 - 20061121
//
// === NOTES ===

#include <tonc.h>

#ifndef __MODE7__
#define __MODE7__

// === CONSTANTS ======================================================

// === CONSTANTS ======================================================


#define M7_D		256		//!< Focal length
#define M7_D_SHIFT	  8		//!< Focal shift
#define M7O_NORM	  2		//!< Object renormalization shift (by /4)

// View frustrum limits
#define M7_LEFT		(-120)		//!< Viewport left
#define M7_RIGHT	 120		//!< Viewport right
#define M7_TOP		  80		//!< Viewport top (y-axis up)
#define M7_BOTTOM	(-80)		//!< Viewport bottom (y-axis up!)
#define M7_NEAR		  24		//!< Near plane (objects)
#define M7_FAR		 512		//!< Far plane (objects)

#define M7_FAR_BG	 768		//!< Far plane (floor)


// === CLASSES ========================================================

//! 3D sprite struct
typedef struct M7_SPRITE
{
	VECTOR pos;		//!< World position.
	POINT anchor;	//!< Sprite anchor.
	OBJ_ATTR obj;	//!< Object attributes.
	s16 phi;		//!< Azimuth angle.
	u8 obj_id;		//!< Object index.
	u8 aff_id;		//!< OBJ_AFFINE index.
	TILE *tiles;	//!< Gfx pointer.
	VECTOR pos2;	//!< Position in cam space (subject to change)
	// To add: shade object
} M7_SPRITE;

//! 3D camera struct
typedef struct M7_CAM 
{
	VECTOR pos;		//!< World position.
	int theta;		//!< Polar angle.
	int phi;		//!< Azimuth angle.
	VECTOR u;		//!< local x-axis (right)
	VECTOR v;		//!< local y-axis (up)
	VECTOR w;		//!< local z-axis (back)
} M7_CAM;


//! One struct to bind them all
typedef struct M7_LEVEL
{
	M7_CAM *camera;			//!< Camera variables
	BG_AFFINE *bgaff;		//!< Affine parameter array
	M7_SPRITE *sprites;		//!< 3D sprites
	int horizon;			//!< Horizon scanline (sorta)
	u16 bgcnt_sky;			//!< BGxCNT for backdrop
	u16 bgcnt_floor;		//!< BGxCNT for floor
} M7_LEVEL;

// === GLOBALS ========================================================

extern M7_LEVEL m7_level;

// === PROTOTYPES =====================================================

void m7_init(M7_LEVEL *level, M7_CAM *cam, BG_AFFINE bg_aff[], 
	M7_SPRITE sprites[], u16 skycnt, u16 floorcnt);
void m7_prep_horizon(M7_LEVEL *level);
void m7_update_sky(const M7_LEVEL *level);

// Camera functions
void m7_rotate(M7_CAM *cam, int theta, int phi);
void m7_translate_level(M7_CAM *cam, const VECTOR *dir);
void m7_translate_local(M7_CAM *cam, const VECTOR *dir);
void m7_translate_global(M7_CAM *cam, const VECTOR *dir);


INLINE int m7_horizon_line(const M7_LEVEL *level);

// IWRAM functions
IWRAM_CODE void m7_prep_affines(M7_LEVEL *level);
IWRAM_CODE void m7_prep_sprite(M7_LEVEL *level, M7_SPRITE *spr);

IWRAM_CODE void m7_hbl_floor();

// === INLINES=========================================================

//! Get the scanline at which the horizon lies
INLINE int m7_horizon_line(const M7_LEVEL *level)
{	return clamp(level->horizon, 0, 228);	}

#endif	// __MODE7__

// EOF
