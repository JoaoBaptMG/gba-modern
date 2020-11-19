//
// m7_isrs.iwram.c
// Separate file for HBL interrupts because apparently it screws up 
//   on hardware now.

#include <tonc.h>

#include "m7_demo.h"


// Perspective zoom for this scanline
// Note that this is actually wrong, because 
// the hblank occurs AFTER REG_VCOUNT is drawn, screwing up 
// at scanline 0. Oh well.

///////////////////////////////////////////////////////////////////////

// --- Type A ---
// By the numbers: everything nice and .8 fixed point. Result: blocky
// - (offset * zoom) * rotate
// - lambda is .8f for xs and ys
// - M7_D multiplied before shift
void m7_hbl_a()
{
	FIXED lam, xs, ys;

	lam= cam_pos.y*lu_div(REG_VCOUNT)>>16;	// .8*.16/.16 = .8

	// Calculate offsets (.8)
	xs= 120*lam;
	ys= M7_D*lam;

	REG_BG2PA= (g_cosf*lam)>>8;
	REG_BG2PC= (g_sinf*lam)>>8;

	REG_BG2X = cam_pos.x - ( (xs*g_cosf-ys*g_sinf)>>8 );
	REG_BG2Y = cam_pos.z - ( (xs*g_sinf+ys*g_cosf)>>8 );	
}


// --- Type B ---
// Partially correct, but with jaggies
// - (offset * zoom) * rotate
// - lambda is .12f for xs and ys
// - 120 multiplied before shift in xs
// * .16 lambda wouldn't work due to 
void m7_hbl_b()
{
	FIXED lam, xs, ys;

	lam= cam_pos.y*lu_div(REG_VCOUNT)>>12;	// .8*.16/.8 = .12

	// Calculate offsets (.12f)
	xs= 120*lam;
	ys= M7_D*lam;

	REG_BG2PA= (g_cosf*lam)>>12;
	REG_BG2PC= (g_sinf*lam)>>12;

	REG_BG2X = cam_pos.x - ( (xs*g_cosf-ys*g_sinf)>>12 );
	REG_BG2Y = cam_pos.z - ( (xs*g_sinf+ys*g_cosf)>>12 );	
}

// --- Type C ---
//'proper' mode 7, with mixed fixed-point. Smooth
// - offset * (zoom * rotate)
// - lambda is .12f for xs and ys
// - 120 multiplied before shift in xs
// * .12 lambda,lcf,lsf would work too.
void m7_hbl_c()
{
	FIXED lam, lcf, lsf, lxr, lyr;

	lam= cam_pos.y*lu_div(REG_VCOUNT)>>12;	// .8*.16 /.12 = 20.12
	lcf= lam*g_cosf>>8;						// .12*.8 /.8 = .12
	lsf= lam*g_sinf>>8;						// .12*.8 /.8 = .12
	
	REG_BG2PA= lcf>>4;
	REG_BG2PC= lsf>>4;

	// Offsets
	// Note that the lxr shifts down first! 

	// horizontal offset
	lxr= 120*(lcf>>4);		lyr= (M7_D*lsf)>>4;
	REG_BG2X= cam_pos.x - lxr + lyr;

	// vertical offset
	lxr= 120*(lsf>>4);		lyr= (M7_D*lcf)>>4;	
	REG_BG2Y= cam_pos.z - lxr - lyr;
}
