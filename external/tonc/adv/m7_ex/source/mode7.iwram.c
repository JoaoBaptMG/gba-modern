//
//  Mode 7 functions (IWRAM)
//
//! \file mode7.iwram.c
//! \author J Vijn
//! \date 20061111 - 20061112
//
// === NOTES ===

#include <tonc.h>

#include "mode7.h"


// === FUNCTIONS ======================================================

IWRAM_CODE void m7_hbl_floor()
{
	int vc= REG_VCOUNT;
	int horz= m7_level.horizon;

	if(!IN_RANGE(vc, horz, SCREEN_HEIGHT) )
		return;

	if(vc == horz)
	{
		BFN_SET(REG_DISPCNT, DCNT_MODE1, DCNT_MODE);
		REG_BG2CNT= m7_level.bgcnt_floor;
	}		
		
	BG_AFFINE *bga= &m7_level.bgaff[vc+1];
	REG_BG_AFFINE[2] = *bga;

	// A distance fogging with high marks for hack-value
	// Remember that we used pb to store the scale in, 
	// so the blend is basically lambda/64 = distance * 2
	// which will do nicely
	u32 ey= bga->pb*6>>12;
	if(ey>16)
		ey= 16;

	REG_BLDALPHA= BLDA_BUILD(16-ey, ey);
}

IWRAM_CODE void m7_prep_affines(M7_LEVEL *level)
{
	if(level->horizon >= SCREEN_HEIGHT)
		return;

	int ii, ii0= (level->horizon>=0 ? level->horizon : 0);

	M7_CAM *cam= level->camera;
	FIXED xc= cam->pos.x, yc= cam->pos.y, zc=cam->pos.z;

	BG_AFFINE *bga= &level->bgaff[ii0];

	FIXED yb, zb;			// b' = Rx(theta) *  (L, ys, -D)
	FIXED cf, sf, ct, st;	// sines and cosines
	FIXED lam, lcf, lsf;	// scale and scaled (co)sine(phi)
	cf= cam->u.x;      sf= cam->u.z;
	ct= cam->v.y;      st= cam->w.y;
	for(ii= ii0; ii<SCREEN_HEIGHT; ii++)
	{
		yb= (ii-M7_TOP)*ct + M7_D*st;
		lam= DivSafe( yc<<12,  yb);		// .12f

		lcf= lam*cf>>8;					// .12f
		lsf= lam*sf>>8;					// .12f

		bga->pa= lcf>>4;				// .8f
		bga->pc= lsf>>4;				// .8f

		// lambda·Rx·b
		zb= (ii-M7_TOP)*st - M7_D*ct;	// .8f
		bga->dx= xc + (lcf>>4)*M7_LEFT - (lsf*zb>>12);	// .8f
		bga->dy= zc + (lsf>>4)*M7_LEFT + (lcf*zb>>12);	// .8f

		// hack that I need for fog. pb and pd are unused anyway
		bga->pb= lam;
		bga++;
	}
	level->bgaff[SCREEN_HEIGHT]= level->bgaff[0];
}

//! Setup an object's attr/affine with the right attributes
/*! \param level	Mode 7 level data
*	\param spr		3D sprite to calculate for
*/
IWRAM_CODE void m7_prep_sprite(M7_LEVEL *level, M7_SPRITE *spr)
{
	M7_CAM *cam= level->camera;
	VECTOR vr, vc;		// Difference and inverted-cam vector
	int sx, sy;			// Object size
	RECT rect;			// Object rectangle 

	// Convert to camera frame
	vec_sub(&vr, &spr->pos, &cam->pos);
	vc.x=  vec_dot(&vr, &cam->u);
	vc.y= -vec_dot(&vr, &cam->v);
	vc.z= -vec_dot(&vr, &cam->w);
	spr->pos2= vc;

	OBJ_ATTR *obj= &spr->obj;
	sx= obj_get_width(obj);
	sy= obj_get_height(obj);
	
	// --- Check with viewbox ---
	do 
	{
		if(M7_NEAR*256 > vc.z || vc.z > M7_FAR*256)
			break;	

		rect.left= vc.x - spr->anchor.x*(256>>M7O_NORM);		
		rect.right= rect.left + sx*(256>>M7O_NORM);

		if(M7_LEFT*vc.z > rect.right*M7_D || rect.left*M7_D > M7_RIGHT*vc.z)
			break;	

		rect.top= vc.y - spr->anchor.y*(256>>M7O_NORM);		
		rect.bottom= rect.top + sy*(256>>M7O_NORM);

		if(-M7_TOP*vc.z > rect.bottom*M7_D || rect.top*M7_D > -M7_BOTTOM*vc.z)
			break;	

		OBJ_AFFINE *oa= &obj_aff_mem[spr->aff_id];
		oa->pa= oa->pd= vc.z>>(M7_D_SHIFT-M7O_NORM);	// normalized lambda
		oa->pb= oa->pc= 0;

		FIXED scale= DivSafe(M7_D<<16, vc.z);	// (.16 / .8) = .8 

		// Base anchoring equation:
		// x = q0 - s - A(p0 - s/2)
		// In this case A = 1/lam; and q0 = xc/lam
		// -> x = (xc - p0 + s/2)/lam - s + screen/2
		int xscr, yscr;
		xscr  = spr->anchor.x*256 - sx*128;				// .8
		xscr  = (vc.x - (xscr>>M7O_NORM))*scale>>16;	// .0
		xscr += -sx - M7_LEFT;

		yscr  = spr->anchor.y*256 - sy*128;				// .8
		yscr  = (vc.y - (yscr>>M7O_NORM))*scale>>16;	// .0
		yscr += -sy + M7_TOP;

		obj_unhide(obj, ATTR0_AFF_DBL);
		obj_set_pos(obj, xscr, yscr);

		return;
	}
	while(0);

	obj_hide(obj);

	// TODO: add shadow
}

// EOF
