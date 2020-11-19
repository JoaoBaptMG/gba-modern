//
//! \file txt_se2.c
//!   Screen entry text demo
//! \date 20051028 - 20060103
//! \author cearn
//
// === NOTES ===

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <tonc.h>

#define LEGACY_TEXT_V11
#include <tonc_legacy.h>

#include "gba_pic.h"

// --------------------------------------------------------------------
// CONSTANTS & STRUCTS
// --------------------------------------------------------------------


int gtimes[5];

const char *strs[5]= 
{	"u16 array", "u32 array", "memcpy", "memcpy32", "DMA32" };

// hw : 674978, 260299, 195171, 86846, 76902
// no$: 672162, 259309, 194608, 85283, 76901
// vba: 557081, 192183, 160367, 80049,   222


// --------------------------------------------------------------------
// FUNCTIONS 
// --------------------------------------------------------------------


// copy via u16 array
void test_0(u16 *dst, const u16 *src, u32 len)
{
	u32 ii;
	profile_start();
	for(ii=0; ii<len/2; ii++)
		dst[ii]= src[ii];
	gtimes[0]= profile_stop();
}

// copy via u32 array
void test_1(u32 *dst, const u32 *src, u32 len)
{
	u32 ii;
	profile_start();
	for(ii=0; ii<len/4; ii++)
		dst[ii]= src[ii];
	gtimes[1]= profile_stop();
}

// copy via memcpy
void test_2(void *dst, const void *src, u32 len)
{
	profile_start();
	memcpy(dst, src, len);
	gtimes[2]= profile_stop();
}

// copy via my own memcpy32
void test_3(void *dst, const void *src, u32 len)
{
	profile_start();
	memcpy32(dst, src, len/4);
	gtimes[3]= profile_stop();
}

// copy using DMA
void test_4(void *dst, const void *src, u32 len)
{
	profile_start();
	dma3_cpy(dst, src, len);
	gtimes[4]= profile_stop();
}

int main()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	test_0((u16*)vid_mem, (const u16*)gba_picBitmap, gba_picBitmapLen);
	test_1((u32*)vid_mem, (const u32*)gba_picBitmap, gba_picBitmapLen);
	test_2(vid_mem, gba_picBitmap, gba_picBitmapLen);
	test_3(vid_mem, gba_picBitmap, gba_picBitmapLen);
	test_4(vid_mem, gba_picBitmap, gba_picBitmapLen);

	// clear the screenblock I'm about to use
	SBB_CLEAR(7);

	// init map text
	txt_init_std();
	txt_init_se(0, BG_SBB(7), 0, CLR_YELLOW, 0);

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	// print results
	int ii;
	char str[32];
	for(ii=0; ii<5; ii++)
	{
		siprintf(str, "%12s %6d", strs[ii], gtimes[ii]);
		se_puts(8, 8+8*ii, str, 0);
	}

	while(1)
		VBlankIntrWait();

	return 0;
}
