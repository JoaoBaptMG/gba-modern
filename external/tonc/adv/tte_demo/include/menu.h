//
// Basic function menu
//
//! \file menu.h
//! \author J Vijn
//! \date 20070926 - 20070926
//
/* === NOTES ===
*/


#ifndef __MENU_H__
#define __MENU_H__

#include <tonc.h>
#include "menu.h"


// --------------------------------------------------------------------
// CONSTANTS 
// --------------------------------------------------------------------


#define MENU_ACT_STD		0x00	//!< Standard run: wait/clear/desc/run.
#define MENU_ACT_ALL		0x01	//!< Run all with waits between items.
#define MENU_ACT_AUTO		0x02	//!< Run all without waits.
//#define MENU_ACT_ALL		0x10	//!< Run all in sequence


#define MENU_PAL_TEXT		0x09
#define MENU_PAL_TYPE_BASE	0x0C
#define MENU_PAL_INK		0x11
#define MENU_PAL_SHADOW		0x12
#define MENU_PAL_NOTE		0x13
#define MENU_PAL_KEYW		0x14
#define MENU_PAL_CMT		0x15
#define MENU_PAL_STR		0x16
#define MENU_PAL_TEXT_BASE	0x18

#define MENU_CLR_WINDOW	RGB15(25,25,25)
#define MENU_CLR_TEXT	RGB15( 4, 4, 4)
#define MENU_CLR_OFF	RGB15(16,16,16)
#define MENU_CLR_INK	RGB15(31,31, 0)
#define MENU_CLR_SHADOW	RGB15(31,16, 0)
#define MENU_CLR_SEL	RGB15(28, 8, 8)
#define MENU_CLR_KEYW	RGB15( 8,16,31)
#define MENU_CLR_CMT	RGB15( 8,24, 8)
#define MENU_CLR_STR	RGB15(24,8, 24)

/*
#define MENU_PAL_FRAME		16			
#define MENU_PAL_PAPER		17
#define MENU_PAL_STD		18
#define MENU_PAL_OFF		19
#define MENU_PAL_SEL		20
		
#define MENU_PAL_ITEM0		24

#define MENU_CLR_FRAME	RGB15(16,16,16)


*/


// --------------------------------------------------------------------
// TYPES 
// --------------------------------------------------------------------


typedef struct TMenuItem
{
	const char *title;			//!< Title for menu (short)
	const char *description;	//!< Description
	void (*proc)(void);			//!< Procedure for menu action (if any)
	u8 type;					//<! Menu type (unused)
	u8 state;					//!< Menu state (unused)
} TMenuItem;

typedef struct TMenu
{
	TMenuItem	*items;			//!< Menu items
	u16		itemCount;			//!< Number of menu items
	s16		sel;				//!< Current selection
	u32		state;				//!< (unused)
} TMenu;


// --------------------------------------------------------------------
// PROTOTYPES 
// --------------------------------------------------------------------

void menu_create(TMenu *menu, const TMenuItem *items, u32 count);

void menu_init(TMenu *menu);
void menu_run(TMenu *menu);
void menu_act(TMenu *menu);
void menu_draw(const TMenu *menu);
void menu_update_page(const TMenu *menu);
void menu_update_item(const TMenu *menu, int sel);

#endif	// __MENU_H__

// EOF
