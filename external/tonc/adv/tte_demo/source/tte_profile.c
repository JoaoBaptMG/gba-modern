//
//  TTE profiling
//
//! \file tte_profile.c
//! \author J Vijn
//! \date 20080304 - 20080311
//
// === NOTES ===


#include <tonc.h>
#include <stdio.h>
#include <string.h>

#include "menu.h"

#include "fonts.h"


/* Testing for:				

// DKA r21
							78			194				194/ WAITCNT=std
null					 17305 / 221	 42881 / 221	 26698 / 137
se_drawg				 44789 / 574	111253 / 595	 68647 / 353
se_drawg_w8h16			 28955 / 371	 71871 / 370	 43621 / 224
ase_drawg				 60389 / 774	150053 / 773	 93867 / 438
ase_drawg_w8h16			 34493 / 442	 85645 / 458	 51963 / 267
chr4c_drawg_b1cts_base					591596 / 3049
chr4c_drawg_b1cts		153993 / 1974	396617 / 2044	247556 / 1276
chr4c_drawg_b1cts_fast	 48264 / 618	122489 / 631	104366 / 537
bmp8_drawg_b1cts_base					538387 / 2775
bmp8_drawg_b1cts		158708 / 2034	403312 / 2078	247326 / 1274
bmp8_drawg_b1cts_fast	 47669 / 611	120208 / 619	102085 / 526
bmp16_drawg_b1cts_base					476487 / 2456
bmp16_drawg_b1cts		115841 / 1485	291759 / 1503	190617 / 982
obj_drawg				 37195 / 476	 92351 / 476	 54970 / 283


// DKA r22 (also with utf-8)
										194				diff
null									 46779 / 241	+ 20
se_drawg								119031 / 613	+ 18
se_drawg_w8h16							 76739 / 395	+ 25
ase_drawg								158219 / 815	+ 42
ase_drawg_w8h16							 94005 / 484	+ 26
chr4c_drawg_b1cts_base					566192 / 2918	-131
chr4c_drawg_b1cts						401765 / 2070	+ 30	Wut ?
chr4c_drawg_b1cts_fast					121731 / 627	-  4
bmp8_drawg_b1cts_base					559134 / 2882	+107
bmp8_drawg_b1cts						399556 / 2059	- 18
bmp8_drawg_b1cts_fast					124688 / 642	+ 23	This is UTF overhead
bmp16_drawg_b1cts_base					473405 / 2440	+ 16
bmp16_drawg_b1cts						291759 / 1511	+  8
obj_drawg								 88872 / 458	+ 18


*/



CSTR glados[]= 
{
"Please note that we have added a consequence for failure. "
	"Any contact with the chamber floor will result in an "
	"'unsatisfactory' mark on your official testing record "
	"followed by death. Good luck!", 
"Assume the party escort submission "
	"position or you will miss the party.", 
"Okay. The test is over. You win. "
	"Go back to the recovery annex. "
	"For your cake.", 
"[pain noise] You think you're doing "
"some damage? Two plus two is ten... "
	"IN BASE FOUR! I'M FINE!", 
"I invited your best friend the companion "
"cube. Of course, he couldn't come because you murdered him."
};

typedef struct TProfiler TProfiler;

struct TProfiler
{
	const char	*str;
	void		(*proc)(TProfiler *prof, const char *str);	
	int			time;
};


void prof_null_drawg(TProfiler *prof, const char *str);
void prof_se_drawg_w8h16(TProfiler *prof, const char *str);
void prof_se_drawg(TProfiler *prof, const char *str);
void prof_ase_drawg_w8h16(TProfiler *prof, const char *str);
void prof_ase_drawg(TProfiler *prof, const char *str);

void prof_chr4c_drawg_b1cts_base(TProfiler *prof, const char *str);
void prof_chr4c_drawg_b1cts(TProfiler *prof, const char *str);
void prof_chr4c_drawg_b1cts_fast(TProfiler *prof, const char *str);

void prof_bmp8_drawg_b1cts_base(TProfiler *prof, const char *str);
void prof_bmp8_drawg_b1cts(TProfiler *prof, const char *str);
void prof_bmp8_drawg_b1cts_fast(TProfiler *prof, const char *str);

void prof_bmp16_drawg_b1cts_base(TProfiler *prof, const char *str);
void prof_bmp16_drawg_b1cts(TProfiler *prof, const char *str);
//void prof_bmp16_drawg(TProfiler *prof, const char *str);
void prof_obj_drawg(TProfiler *prof, const char *str);


TProfiler gProfs[]= 
{
	{	"null",				prof_null_drawg, 			0 }, 
	{	"se",				prof_se_drawg, 				0 }, 
	{	"se_w8h16",			prof_se_drawg_w8h16, 		0 }, 
	{	"ase",				prof_ase_drawg, 			0 }, 
	{	"ase_w8h16",		prof_ase_drawg_w8h16, 		0 }, 
	{	"chr4c_b1cts_base",	prof_chr4c_drawg_b1cts_base,	0 }, 
	{	"chr4c_b1cts",		prof_chr4c_drawg_b1cts	, 	0 }, 
	{	"chr4c_b1cts_fast",	prof_chr4c_drawg_b1cts_fast, 0 }, 
	{	"bmp8_b1cts_base",	prof_bmp8_drawg_b1cts_base, 0 }, 
	{	"bmp8_b1cts",		prof_bmp8_drawg_b1cts, 		0 }, 
	{	"bmp8_b1cts_fast",	prof_bmp8_drawg_b1cts_fast, 0 }, 
	{	"bmp16_b1cts_base",	prof_bmp16_drawg_b1cts_base,0 }, 
	{	"bmp16_b1cts",		prof_bmp16_drawg_b1cts, 	0 }, 
	//{	"bmp16",			prof_bmp16_drawg, 			0 }, 
	{	"obj",				prof_obj_drawg, 			0 }, 
};

TFont gFont;

// --------------------------------------------------------------------
// Additional renderers
// --------------------------------------------------------------------


void null_drawg(uint gid)
{
}

//! Simple version of chr4 renderer. 3049 c/glyph (2622 with inline schr4c_plot)
void chr4c_drawg_b1cts_base(uint gid)
{
	TTE_BASE_VARS(tc, font);
	TTE_CHAR_VARS(font, gid, u8, srcD, srcL, charW, charH);
	uint x0= tc->cursorX, y0= tc->cursorY;
	uint srcP= font->cellH;

	u32 ink= tc->cattr[TTE_INK], raw;

	uint ix, iy, iw;
	for(iw=0; iw<charW; iw += 8)
	{	
		for(iy=0; iy<charH; iy++)
		{
			raw= srcL[iy];
			for(ix=0; raw>0; raw>>=1, ix++)
				if(raw&1)
					schr4c_plot(&tc->dst, x0+ix, y0+iy, ink);

		}
		srcL += srcP;
		x0 += 8;
	}
}

//! Simple version of bmp8 renderer.
void bmp8_drawg_b1cts_base(uint gid)
{
	TTE_BASE_VARS(tc, font);
	TTE_CHAR_VARS(font, gid, u8, srcD, srcL, charW, charH);
	uint x0= tc->cursorX, y0= tc->cursorY;
	uint srcP= font->cellH;

	u32 ink= tc->cattr[TTE_INK], raw;

	uint ix, iy, iw;
	for(iw=0; iw<charW; iw += 8)
	{	
		for(iy=0; iy<charH; iy++)
		{
			raw= srcL[iy];
			for(ix=0; raw>0; raw>>=1, ix++)
				if(raw&1)
					sbmp8_plot(&tc->dst, x0+ix, y0+iy, ink);

		}
		srcL += srcP;
		x0 += 8;
	}
}

//! Simple version of bmp16 renderer.
void bmp16_drawg_b1cts_base(uint gid)
{
	TTE_BASE_VARS(tc, font);
	TTE_CHAR_VARS(font, gid, u8, srcD, srcL, charW, charH);
	uint x0= tc->cursorX, y0= tc->cursorY;
	uint srcP= font->cellH;

	u32 ink= tc->cattr[TTE_INK], raw;

	uint ix, iy, iw;
	for(iw=0; iw<charW; iw += 8)
	{	
		for(iy=0; iy<charH; iy++)
		{
			raw= srcL[iy];
			for(ix=0; raw>0; raw>>=1, ix++)
				if(raw&1)
					sbmp16_plot(&tc->dst, x0+ix, y0+iy, ink);

		}
		srcL += srcP;
		x0 += 8;
	}
}


// --------------------------------------------------------------------
// Profiler routines
// --------------------------------------------------------------------


void prof_draws(TProfiler *prof, const char *str)
{
	tte_set_pos(0, 0);
	tte_write(prof->str);
	tte_write(": \n");
	VBlankIntrWait();

	profile_start();
	tte_write(str);

	prof->time=  profile_stop();
}


void prof_null_drawg(TProfiler *prof, const char *str)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;
	
	tte_init_bmp(3, &gFont, null_drawg);

	prof_draws(prof, str);	
}

void prof_se_drawg(TProfiler *prof, const char *str)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG2;

	int charH= gFont.charH;			gFont.charH= 16;
	const u8 *widths= gFont.widths;	gFont.widths= NULL;
	
	tte_init_se(2, BG_CBB(0)|BG_SBB(24), 0, CLR_YELLOW, 0, 
		&gFont, se_drawg);

	prof_draws(prof, str);

	gFont.charH= charH;
	gFont.widths= widths;
}

void prof_se_drawg_w8h16(TProfiler *prof, const char *str)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG2;

	int charH= gFont.charH;			gFont.charH= 16;
	const u8 *widths= gFont.widths;	gFont.widths= NULL;
	
	tte_init_se(2, BG_CBB(0)|BG_SBB(24), 0, CLR_YELLOW, 0, 
		&gFont, se_drawg_w8h16);

	prof_draws(prof, str);

	gFont.charH= charH;
	gFont.widths= widths;
}

void prof_ase_drawg(TProfiler *prof, const char *str)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE1 | DCNT_BG2;

	int charH= gFont.charH;			gFont.charH= 16;
	const u8 *widths= gFont.widths;	gFont.widths= NULL;
	
	tte_init_ase(2, BG_CBB(0)|BG_SBB(24) | BG_AFF_32x32, 0, CLR_YELLOW, 0, 
		&gFont, ase_drawg);

	prof_draws(prof, str);

	gFont.charH= charH;
	gFont.widths= widths;	
}

void prof_ase_drawg_w8h16(TProfiler *prof, const char *str)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE1 | DCNT_BG2;

	int charH= gFont.charH;			gFont.charH= 16;
	const u8 *widths= gFont.widths;	gFont.widths= NULL;
	
	tte_init_ase(2, BG_CBB(0)|BG_SBB(24) | BG_AFF_32x32, 0, CLR_YELLOW, 0, 
		&gFont, ase_drawg_w8h16);

	prof_draws(prof, str);

	gFont.charH= charH;
	gFont.widths= widths;
	
}

void prof_chr4c_base(TProfiler *prof, const char *str, fnDrawg proc)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG2;
	
	tte_init_chr4c(2, BG_CBB(0)|BG_SBB(24), 0xF000, 0x0201, CLR_YELLOW,
		&gFont, proc);

	prof_draws(prof, str);
}

void prof_chr4c_drawg_b1cts_base(TProfiler *prof, const char *str)
{
	prof_chr4c_base(prof, str, chr4c_drawg_b1cts_base);
}

void prof_chr4c_drawg_b1cts(TProfiler *prof, const char *str)
{
	prof_chr4c_base(prof, str, chr4c_drawg_b1cts);
}

void prof_chr4c_drawg_b1cts_fast(TProfiler *prof, const char *str)
{
	prof_chr4c_base(prof, str, (fnDrawg)chr4c_drawg_b1cts_fast);
}

void prof_bmp8_base(TProfiler *prof, const char *str, fnDrawg proc)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;
	vid_page= vid_mem;
	
	tte_init_bmp(4, &gFont, proc);

	prof_draws(prof, str);	
}

void prof_bmp8_drawg_b1cts_base(TProfiler *prof, const char *str)
{
	prof_bmp8_base(prof, str, bmp8_drawg_b1cts_base);
}

void prof_bmp8_drawg_b1cts(TProfiler *prof, const char *str)
{
	prof_bmp8_base(prof, str, bmp8_drawg_b1cts);
}

void prof_bmp8_drawg_b1cts_fast(TProfiler *prof, const char *str)
{
	prof_bmp8_base(prof, str, (fnDrawg)bmp8_drawg_b1cts_fast);
}


void prof_bmp16_base(TProfiler *prof, const char *str, fnDrawg proc)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;
	
	tte_init_bmp(3, &gFont, proc);

	prof_draws(prof, str);	
}

void prof_bmp16_drawg_b1cts_base(TProfiler *prof, const char *str)
{
	prof_bmp16_base(prof, str, bmp16_drawg_b1cts_base);
}

void prof_bmp16_drawg_b1cts(TProfiler *prof, const char *str)
{
	prof_bmp16_base(prof, str, bmp16_drawg_b1cts);
}

void prof_bmp16_drawg(TProfiler *prof, const char *str)
{
	extern void bmp16_drawg(uint gid);
	prof_bmp16_base(prof, str, bmp16_drawg);
}


void prof_obj_drawg(TProfiler *prof, const char *str)
{
	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
	
	tte_init_obj(&oam_mem[127], ATTR0_TALL, ATTR1_SIZE_8, 0, CLR_YELLOW, 0, 
		&gFont, obj_drawg);

	prof_draws(prof, str);	
}


//! Testing rendering speeds.
void test_speeds()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	gFont= vwf_default;

	int ii;
	const char *str= glados[0];
	int len= strlen(str);

	for(ii=0; ii<countof(gProfs); ii++)
	{
		gProfs[ii].proc(&gProfs[ii], str);
		key_wait_till_hit(KEY_ANY);
	}

	RegisterRamReset(RESET_GFX);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG2;

	tte_init_se_default(2, BG_CBB(0) | BG_SBB(24));
	tte_init_con();
	pal_bg_bank[14][1]= CLR_ORANGE;

	iprintf("%d\n", len);
	for(ii=0; ii<countof(gProfs); ii++)
	{
		tte_set_special(SE_PALBANK(14+(ii&1)));
		iprintf("%-17s %6d  %d\n", gProfs[ii].str, gProfs[ii].time, 
			gProfs[ii].time/len);
	}

	key_wait_till_hit(KEY_START);
}

// EOF
