//
// toolbox.c
// 
// Tools source for pageflip
// 
// (20060922-20060922, cearn)
//
// === NOTES ===
// * This is a _small_ set of typedefs, #defines and inlines that can 
//   be found in tonclib, and might not represent the 
//   final forms.

#include "toolbox.h"

// === (tonc_core.c) ==================================================

u16 __key_curr= 0, __key_prev= 0;
COLOR *vid_page= vid_mem_back;


// === (tonc_bitmap.c) ================================================

COLOR *vid_flip()
{
	vid_page= (COLOR*)((u32)vid_page ^ VRAM_PAGE_SIZE);
	REG_DISPCNT ^= DCNT_PAGE;	// update control register	

	return vid_page;
}
