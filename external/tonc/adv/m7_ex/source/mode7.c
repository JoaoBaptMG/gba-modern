//
//  Mode 7 functions (ROM)
//
//! \file mode7.c
//! \author J Vijn
//! \date 20061111 - 20061121
//
// === NOTES ===

#include <limits.h>
#include <tonc.h>

#include "mode7.h"


// === CONSTANTS ======================================================
// === CLASSES ========================================================
// === GLOBALS ========================================================


// === FUNCTIONS ======================================================

void m7_init(M7_LEVEL *level, M7_CAM *cam, BG_AFFINE bgaff[], 
	M7_SPRITE sprites[], u16 skycnt, u16 floorcnt)
{
	level->camera= cam;
	level->bgaff= bgaff;
	level->sprites= sprites;
	level->bgcnt_sky= skycnt;
	level->bgcnt_floor= floorcnt;
	level->horizon= 80;

	REG_BG2CNT= floorcnt;
	REG_BG_AFFINE[2]= bg_aff_default;
}
	
//! Calculate the horizon scanline
void m7_prep_horizon(M7_LEVEL *level)
{
	int horz;
	M7_CAM *cam= level->camera;

	if(cam->v.y != 0)
	{
#ifndef M7_HORZ_INFINITE
		horz= M7_FAR_BG*cam->w.y - cam->pos.y;
		horz= M7_TOP - Div(horz*M7_D, M7_FAR_BG*cam->v.y);
#else
		horz= M7_TOP - Div(cam->w.y*M7_D, cam->v.y);
#endif
	}
	else	// looking straight down (w.y > 0) means horizon at -inf scanline
		horz= cam->w.y > 0 ? INT_MIN : INT_MAX;
	
	level->horizon= horz;
}

void m7_rotate(M7_CAM *cam, int phi, int theta)
{
	theta= clamp(theta, -0x3FFF, 0x4001);
	cam->phi= phi;
	cam->theta= theta;

	FIXED cf, sf, ct, st;

	cf= lu_cos(phi)>>4;		sf= lu_sin(phi)>>4;
	ct= lu_cos(theta)>>4;	st= lu_sin(theta)>>4;
	
	// camera X-axis (right)
	cam->u.x= cf;
	cam->u.y= 0;
	cam->u.z= sf;

	// camera Y-axis (up)
	cam->v.x= sf*st>>8;
	cam->v.y= ct;
	cam->v.z= -cf*st>>8;

	// camera Z-axis (back)
	cam->w.x= -sf*ct>>8;
	cam->w.y= st;
	cam->w.z= cf*ct>>8;
}

//! Translate by \a dir using local frame for x/y, but global z
void m7_translate_level(M7_CAM *cam, const VECTOR *dir)
{
	cam->pos.x += (cam->u.x * dir->x - cam->u.z * dir->z)>>8;
	cam->pos.y += dir->y;
	cam->pos.z += (cam->u.z * dir->x + cam->u.x * dir->z)>>8;
}

//! Translate by \a dir in local frame
void m7_translate_local(M7_CAM *cam, const VECTOR *dir)
{
	// Operator overloading would be real nice right now :/
	cam->pos.x += (cam->u.x * dir->x + cam->v.x * dir->y + cam->w.x * dir->z) >> 8;
	cam->pos.y += ( 0                + cam->v.y * dir->y + cam->w.y * dir->z) >> 8;
	cam->pos.z += (cam->u.z * dir->x + cam->v.z * dir->y + cam->w.z * dir->z) >> 8;
}

//! Translate by \a dir in global frame
void m7_translate_global(M7_CAM *cam, const VECTOR *dir)
{
	vec_add_eq(&cam->pos, dir);
}

//! Update sky-bg position
void m7_update_sky(const M7_LEVEL *level)
{
	REG_BG2HOFS= (level->camera->phi>>6)+M7_LEFT;
	REG_BG2VOFS= -m7_horizon_line(level)-1;
}





// EOF
