//
//! \file txt_obj.c
//!   Object text demo
//! \date 20051028 - 20060930
//! \author cearn
//
// === NOTES ===
// * Yes I know it contains 'hidden' bugs, but sorting them out would 
//   just complicate the code to no end and isn't worth the bother.

#include <tonc.h>

#define LEGACY_TEXT_V11
#include <tonc_legacy.h>


// --------------------------------------------------------------------
// CONSTANTS & STRUCTS
// --------------------------------------------------------------------


#define POS0 (80<<8)
#define GRAV 0x40
#define DAMP 0xD0
#define HWLEN 12

const char hwstr[]= "Hello world!";

typedef struct 
{
	u32 state;
	int tt;
	FIXED fy;
	FIXED fvy;
} PATTERN;


// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------


void pat_bounce(PATTERN *pat)
{
	if(pat->tt <= 0)	// timer's run out: play pattern
	{
		pat->fvy += GRAV;
		pat->fy += pat->fvy;

		// touched floor: bounce
		if(pat->fy > POS0)
		{
			// damp if we still have enough speed
			// otherwise kill movement
			if(pat->fvy > DAMP)
			{
				pat->fy= 2*POS0-pat->fy;
				pat->fvy= DAMP-pat->fvy;
			}
			else
			{
				pat->fy= POS0;
				pat->fvy= 0;
			}
		}
	}
	else	// still in waiting period
		pat->tt--;
}

int main()
{
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2 | DCNT_OBJ;

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	memset16(&vid_mem[88*240], CLR_GREEN, 240);

	// init sprite text
	txt_init_std();
	txt_init_obj(oam_mem, 0xF200, CLR_YELLOW, 0x0E);
	// 12 px between letters
	gptxt->dx= 12;

	// init sprite letters
	OBJ_ATTR *obj= oam_mem;
	obj_puts2(120-12*HWLEN/2, 8, hwstr, 0xF200, obj);

	int ii;
	PATTERN pats[HWLEN];

	for(ii=0; ii<HWLEN; ii++)
	{
		// init patterns
		pats[ii].state=0;
		pats[ii].tt= 3*ii+1;
		pats[ii].fy= -12<<8;
		pats[ii].fvy= 0;
		
		// init sprite position
		obj[ii].attr0 &= ~ATTR0_Y_MASK;
		obj[ii].attr0 |= 160;
	}

	while(1)
	{
		VBlankIntrWait();

		for(ii=0; ii<HWLEN; ii++)
		{
			pat_bounce(&pats[ii]);

			obj[ii].attr0 &= ~ATTR0_Y_MASK;
			obj[ii].attr0 |= (pats[ii].fy>>8)& ATTR0_Y_MASK;
		}
	}

	return 0;
}
