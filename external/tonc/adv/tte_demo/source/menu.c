//
// Basic function menu
//
//! \file menu.c
//! \author J Vijn
//! \date 20070926 - 20071104
//
/* === NOTES ===
*/

#include <string.h>

#include <tonc.h>
#include "menu.h"
#include "menu_gfx.h"

const RECT cMenuMainRect= {  16,  16, 224, 128 };
const RECT cMenuPageRect= { 200, 138, 224, 150 };


void menu_act_clear(const TMenu *menu);
void menu_act_description(const TMenu *menu);


// --------------------------------------------------------------------
// FUNCTIONS 
// --------------------------------------------------------------------

//! Create the menu itself.
void menu_create(TMenu *menu, const TMenuItem *items, u32 count)
{

	menu->items= (TMenuItem*)items;
	menu->itemCount= count;
	menu->sel= 0;
	menu->state= MENU_ACT_STD;
}

//! Prepare for menu run (PONDER: incorporate in menu_run?).
void menu_init(TMenu *menu)
{
	int ii, nn;

	RegisterRamReset(RESET_GFX | RESET_REG_MASK);
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	// --- Init graphic parts ---

	// Setup video mode, palette, text, VRAM
	// Graphic inits
	key_repeat_limits(40, 20);
	
	vid_page= vid_mem;
	tte_init_bmp_default(4);
	tte_init_con();
	tte_set_ink(MENU_PAL_TEXT_BASE);

	// Init palette
	GRIT_CPY(pal_bg_mem, menu_gfxPal);
	pal_bg_mem[0]= CLR_BLACK;

	nn= min(8, menu->itemCount);
	for(ii=0; ii<nn; ii++)
		pal_bg_mem[MENU_PAL_TEXT_BASE+ii]= MENU_CLR_TEXT;

	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;	
}


//! Run the menu.
void menu_run(TMenu *menu)
{
	// Initial Draw 
	menu_draw(menu);

	int sel= clamp(menu->sel, 0, menu->itemCount);
	menu->sel= sel;
	int page= sel>>3, selmax= menu->itemCount;

	// Run menu choices
	while(1)
	{
		VBlankIntrWait();
		key_poll();

		// Change run-type
		menu->state += KEY_TRIBOOL(key_hit, KI_R, KI_L);
		menu->state= wrap(menu->state, 0, 3);

		// move selection and act on move
		if(key_repeat(KEY_DOWN | KEY_UP))
		{
			sel += KEY_TRIBOOL(key_repeat, KI_DOWN, KI_UP);
			sel	 = wrap(sel, 0, selmax);	
		}
		else if(key_repeat(KEY_RIGHT | KEY_LEFT))
		{
			sel += 8*KEY_TRIBOOL(key_repeat, KI_RIGHT, KI_LEFT);
			// page wrap
			sel= wrap(sel, 0, (selmax+7)&~7);
			if(sel >= selmax)
				sel= selmax-1;
		}

		menu->sel= sel;

		if(sel>>3 != page)
		{
			page= sel>>3;
			menu_draw(menu);				
		}
		menu_update_page(menu);

		// if KEY_ACCEPT: break	
		if(key_hit(KEY_ACCEPT))
			break;
	}
}

//! Run the selected menu option(s).
void menu_act(TMenu *menu)
{
	TMenuItem *mi= &menu->items[menu->sel];
	int ii, nn;

	if(mi->proc == NULL)
		return;

	switch(menu->state)
	{
	// Run one demo
	case MENU_ACT_STD:
		{
			menu_act_clear(menu);
			menu_act_description(menu);

			VBlankIntrDelay(5);
			mi->proc();
			VBlankIntrDelay(5);
			break;
		}
	// Run all, without auto-timing at the description
	case MENU_ACT_ALL:		//# TODO: create separate state for this
	case MENU_ACT_AUTO:
		{
			menu_act_clear(menu);
			nn= menu->itemCount;

			for(ii=0; ii<nn; ii++)
			{
				menu->sel= ii;
				mi= &menu->items[menu->sel];
				
				if(mi->proc == NULL)
					continue;

				menu_act_description(menu);

				VBlankIntrDelay(5);
				mi->proc();
				VBlankIntrDelay(5);
			}			
		}	
	}
}

//! Draw the whole menu
void menu_draw(const TMenu *menu)
{
	int ii, nn, page;

	RLUnCompVram(menu_gfxBitmap, vid_page);

	// Draw page
	page= menu->sel/8;	
	nn= min(8, menu->itemCount-page*8);
	TMenuItem *mi= &menu->items[page*8];

	// Plot item strings
	for(ii=0; ii<nn; ii++)
		//tte_printf("#{P:%d,%d;ci:%d}\xC2\x95 %s", 
		tte_printf("#{P:%d,%d;ci:%d}* %s", 
			cMenuMainRect.left, cMenuMainRect.top+14*ii, 
			MENU_PAL_TEXT_BASE+ii, mi[ii].title);

	menu_update_page(menu);

	// Page index.
	char str[32];
	sprintf(str, "%d/%d", page+1, (menu->itemCount+7)/8);
	POINT16 pt= tte_get_text_size(str);
	tte_printf("#{P:%d,138;ci:%d}%s", cMenuPageRect.right-pt.x, 
		MENU_PAL_TEXT, str);
}

//! Update the page for colors
void menu_update_page(const TMenu *menu)
{
	int ii, nn, page;

	// Update menu items
	page= menu->sel/8;	
	nn= min(8, menu->itemCount-page*8);
	for(ii=0; ii<nn; ii++)
		menu_update_item(menu, page*8+ii);

	// update run-type
	COLOR *clr= &pal_bg_mem[MENU_PAL_TYPE_BASE];
	for(ii=0; ii<3; ii++)
		clr[ii]= menu->state==ii ? MENU_CLR_SEL : MENU_CLR_TEXT;
}


//! Draw menu item (kinda; switches palettes to match options currently)
void menu_update_item(const TMenu *menu, int sel)
{
	if(sel >= menu->itemCount)
		return;

	COLOR *clr= &pal_bg_mem[MENU_PAL_TEXT_BASE+(sel&7)];

	if(menu->items[sel].proc == NULL)
	{
		*clr= MENU_CLR_OFF;
		return;
	}

	*clr= (menu->sel == sel) ? MENU_CLR_SEL : MENU_CLR_TEXT;
}


void menu_act_clear(const TMenu *menu)
{
	int ii, nn, page;

	// clear other menu items
	page = menu->sel/8;	
	nn= min(8, menu->itemCount-page*8);
	COLOR *clr= &pal_bg_mem[MENU_PAL_TEXT_BASE];

	for(ii=0; ii<nn; ii++)
		clr[ii]= (menu->sel == page*8+ii) ? MENU_CLR_SEL : MENU_CLR_WINDOW;

	VBlankIntrDelay(15);
	m4_fill(0);
	VBlankIntrDelay(15);
}


//! Handle description stage
void menu_act_description(const TMenu *menu)
{
	TMenuItem *mi= &menu->items[menu->sel];

	if(!mi->description)
		return;

	VBlankIntrWait();

	// --- (re)-init gfx for description ---
	if(menu->state != MENU_ACT_STD)
	{
		RegisterRamReset(RESET_GFX);
		// Erase gfx registers (but not REG_DISPSTAT. That apparently 
		// would be bad)
		memset32((void*)REG_BGCNT, 0, 0x58/4);

		REG_BG_AFFINE[2]= bg_aff_default;
		
		vid_page= vid_mem;
		tte_init_bmp_default(4);
		tte_init_con();

		// Init palette
		GRIT_CPY(pal_bg_mem, menu_gfxPal);
		pal_bg_mem[0]= CLR_BLACK;

		REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;	
	}

	// --- Run description ---

	VBlankIntrWait();

	tte_set_margins(8,8, SCR_W-8, SCR_H-8);
	tte_set_font(&verdana9bFont);
	tte_printf("#{P;ci:" XSTR(MENU_PAL_KEYW) "}%s", mi->title);
	tte_set_font(&vwf_default);
	tte_printf("#{P:8,20;ci:" XSTR(MENU_PAL_INK) "}%s",mi->description);
	tte_set_margins(0, 0, SCR_W, SCR_H);
	
	if(menu->state == MENU_ACT_AUTO)
	{
		u32 len= max(strlen(mi->description), 60);
		VBlankIntrDelay(len);
	}
	else
	{
		while(1)
		{
			VBlankIntrWait();
			key_poll();
			if(key_hit(KEY_ANY))
				break;
		}
	}

	RegisterRamReset(RESET_GFX);
}

// EOF
