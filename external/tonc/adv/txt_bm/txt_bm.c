//
//! \file txt_bm.c
//!   Bitmap text demo (modes 3,4,5)
//! \date 20051009 - 20060930
//! \author cearn
//
// === NOTES ===
// * The current mode is indicated by the readable "mode 3",
//   "mode 4" or "mode 5". The cursor position is displayed in white.
// * D-pad: moves cursor.
// * L,R: switches between modes
// * Start: toggles cursor-string 'opacity' (i.e, between direct 
//   overwrite and clear+overwrite)

#include <stdio.h>
#include <tonc.h>

#define LEGACY_TEXT_V11
#include <tonc_legacy.h>


// --------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------

#define CLR_BD    0x080F

const TILE cursorTile= 
{{	0x0, 0x21, 0x211, 0x2111, 0x21111, 0x2100, 0x1100, 0x21000	}};


// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------


void init_main()
{
	vid_page= vid_mem;

	// init interrupts
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	// init backdrop
	pal_bg_mem[0]= CLR_MAG;
	pal_bg_mem[CLR_BD>>8]= CLR_BD;
	pal_bg_mem[CLR_BD&255]= CLR_BD;
	m3_fill(CLR_BD);

	// init mode 4 pal
	pal_bg_mem[1]= CLR_LIME;
	pal_bg_mem[255]= CLR_WHITE;

	// init cursor
	tile_mem[5][0]= cursorTile;
	pal_obj_mem[1]= CLR_WHITE;
	pal_obj_mem[2]= CLR_GRAY;
}

int main()
{
	init_main();

	txt_init_std();

	// print some string so we know what mode we're at
	m3_puts(8, 8, "mode 3", CLR_CYAN);
	m4_puts(12, 32, "mode 4", 1);
	m5_puts(16, 40, "mode 5", CLR_YELLOW);

	// init variables
	u32 mode=3, bClear=0;
	OBJ_ATTR cursor= { 80, 120, 512, 0 };

	// init video mode
	REG_DISPCNT= DCNT_BG2 | DCNT_OBJ | 3;

	// init cursor string
	char str[32];
	siprintf(str, "o %3d,%3d", cursor.attr1, cursor.attr0);

	while(1)
	{
		VBlankIntrWait();

		oam_mem[0]= cursor;
		key_poll();

		if(key_hit(KEY_START))
			bClear ^= 1;

		// move cursor
		cursor.attr1 += key_tri_horz();
		cursor.attr0 += key_tri_vert();

		// adjust cursor(-string) only if necessary
		if(key_is_down(KEY_ANY))
		{
			if(bClear)
				bm_clrs(80, 112, str, CLR_BD);

			cursor.attr0 &= ATTR0_Y_MASK;
			cursor.attr1 &= ATTR1_X_MASK;
			siprintf(str, "%c %3d,%3d", (bClear?'c':'o'), 
				cursor.attr1, cursor.attr0);
		}
		
		// switch modes
		mode += bit_tribool(key_hit(-1), KI_R, KI_L);
		mode= clamp(mode, 3, 6);

		BFN_SET(REG_DISPCNT, mode, DCNT_MODE);

		bm_puts(80, 112, str, CLR_WHITE);
	}

	return 0;
}
