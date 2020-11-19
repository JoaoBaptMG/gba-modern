//
// swi_demo.c
//
// Demonstrates software interrupt
// Note that swi.c uses inline assembly, which is 
// compiler dependent. The THUMB/ARM thing is nicely
// circumvented by defining the swi_call macro.
//
// (20031228 - 20070809, Cearn)

#include <stdio.h>
#include <tonc.h>

// === swi calls ======================================================

/*
//# NOTE, 20120520 : inline ASM functions now pretty much require
//# proper register-input/output matching, so these don't work anymore.

// Their assembly equivalents can be found in tonc_bios.s

void VBlankIntrWait()
{	swi_call(0x05);	}

int Div(int num, int denom)
{	
	s32 result;
	swi_call_r0(0x06, result);
	return result;
	//swi_call(0x06);	
}

u32 Sqrt(u32 num)
{	swi_call(0x08);	}

s16 ArcTan2(s16 x, s16 y)
{	swi_call(0x0a);	}

void ObjAffineSet(const AFF_SRC *src, void *dst, int num, int offset)
{	swi_call(0x0f);	}
*/


// === swi demos ======================================================

// NOTE!
// To be consistent with general mathematical graphs, the 
// y-axis has to be reversed and the origin moved to the 
// either the bottom or mid of the screen via
// "iy = H - y"
// or
// "iy = H/2 - y"
//
// functions have been scaled to fit the graphs on the 240x160 screen

// y= 2560/x
void div_demo()
{
	int ix, y;
	ASM_BREAK();

	for(ix=1; ix<SCREEN_WIDTH; ix++)
	{
		y= Div(0x0a000000, ix)>>16;
		if(y <= SCREEN_HEIGHT)
			m3_plot(ix, SCREEN_HEIGHT - y, CLR_RED);
	}
	tte_printf("#{P:168,132;ci:%d}div", CLR_RED);
}

// y= 160*sqrt(x/240)
void sqrt_demo()
{
	int ix, y;
	for(ix=0; ix<SCREEN_WIDTH; ix++)
	{
		y= Sqrt(Div(320*ix, 3));
		m3_plot(ix, SCREEN_HEIGHT - y, CLR_LIME);
	}
	tte_printf("#{P:160,8;ci:%d}sqrt", CLR_LIME);
}

// y = 80 + tan((x-120)/16) * (64)*2/pi
// ArcTan2 lies in < -0x4000, 0x4000 >
void arctan2_demo()
{
	int ix, y;
	int ww= SCREEN_WIDTH/2, hh= SCREEN_HEIGHT/2;
	for(ix=0; ix < SCREEN_WIDTH; ix++)
	{
		y= ArcTan2(0x10, ix-ww);
		m3_plot(ix, hh - y/256, CLR_MAG);
	}
	tte_printf("#{P:144,40;ci:%d}atan", CLR_MAG);
	
	// pick some random memory to show the range of arctan2
	OBJ_ATTR *obj= &oam_mem[4];
	obj->attr0= ArcTan2( 0x100,  0x100);	// q0 : 0x2000 (pi*1/4)
	obj->attr1= ArcTan2(-0x100,  0x100);	// q1 : 0x6000 (pi*3/4)
	obj->attr2= ArcTan2(-0x100, -0x100);	// q2 : 0xa000 (pi*5/4 = pi*-3/4)
	obj->fill=  ArcTan2( 0x100, -0x100);	// q3 : 0xe000 (pi*7/4 = pi*-1/4)
}

// wX= 1, wY= 80
// cc= 80*sx*cos(2*pi*alpha/240)
// ss=  1*sy*sin(2*pi*alpha/240)
void aff_demo()
{
	int ix, ss, cc;
	ObjAffineSource af_src= {0x0100, 0x5000, 0};	// sx=1, sy=80, alpha=0
	ObjAffineDest af_dest= {0x0100, 0, 0, 0x0100};	// =I (redundant)

	for(ix=0; ix<SCREEN_WIDTH; ix++)
	{
		ObjAffineSet(&af_src, &af_dest, 1, BG_AFF_OFS);
		cc= 80*af_dest.pa>>8; 
		ss= af_dest.pc>>8;
		m3_plot(ix, 80 - cc, CLR_YELLOW);
		m3_plot(ix, 80 - ss, CLR_CYAN);
		// 0x010000/0xf0 = 0x0111.111...
		af_src.alpha += 0x0111;			// acceptable rounding error
	}

	tte_printf("#{P:48,38;ci:%d}cos", CLR_YELLOW);
	tte_printf("#{P:72,20;ci:%d}sin", CLR_CYAN);
}

// === main ===========================================================

int main()
{
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

	tte_init_bmp_default(3);
	tte_init_con();

	div_demo();
	sqrt_demo();
	aff_demo();

	arctan2_demo();
	
	while(1);
	
	return 0;
}
