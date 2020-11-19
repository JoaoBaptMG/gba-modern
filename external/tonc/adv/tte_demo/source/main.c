
#include <stdio.h>
#include <string.h>
#include <tonc.h>

#include "menu.h"
#include "fonts.h"
#include "all_gfx.h"

// --------------------------------------------------------------------
// PROTOTYPES
// --------------------------------------------------------------------

// --- bmp16, bmp8, chr4 slideshow demos ---

void test_chr4c_slide();
void test_bmp16_slide();
void test_bmp8_slide();
void test_tte_se4();
void test_tte_ase();
void test_tte_obj();
void test_fonts();
void test_speeds();


void slide_test();
void slide_show(int mode);

void win_textbox(int bgnr, int left, int top, int right, int bottom, int bldy);


// --------------------------------------------------------------------
// TESTERS
// --------------------------------------------------------------------


//! Regular tilemap text.
void test_tte_se4()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	// Base TTE init for tilemaps
	tte_init_se(
		0,						// Background number (0-3)
		BG_CBB(0)|BG_SBB(31),	// BG control
		0,						// Tile offset (special cattr)
		CLR_YELLOW,				// Ink color
		14,						// BitUnpack offset (on-pixel = 15)
		NULL,					// Default font (sys8) 
		NULL);					// Default renderer (se_drawg_s)

	// Init some colors
	pal_bg_bank[1][15]= CLR_RED;
	pal_bg_bank[2][15]= CLR_GREEN;
	pal_bg_bank[3][15]= CLR_BLUE;
	pal_bg_bank[4][15]= CLR_WHITE;
	pal_bg_bank[5][15]= CLR_MAG;

	pal_bg_bank[4][14]= CLR_GRAY;


	// --- Print some text ---

	// "Hello world in different colors"
	tte_write("\n Hello world! in yellow\n");
	tte_write(" #{cx:0x1000}Hello world! in red\n");
	tte_write(" #{cx:0x2000}Hello world! in green\n");

	// Color use explained
	tte_set_pos(8, 64);
	tte_write("#{cx:0x0000}C#{cx:0x1000}o#{cx:0x2000}l");
	tte_write("#{cx:0x3000}o#{cx:0x4000}r#{cx:0x5000}s");
	tte_write("#{cx:0} provided by \\#{cx:#}.");


	// --- Init for 8x16 font and print something ---

	GRIT_CPY(&tile_mem[0][256], cyber16Glyphs);	// Load tiles
	tte_set_font(&cyber16Font);					// Attach font
	tte_set_drawg(se_drawg_w8h16);				// Attach renderer
	tte_set_special(SE_BUILD(256, 4, 0, 0));	// Set special cattr

	tte_write("#{P:8,80}Also available in 8x16");

	key_wait_till_hit(KEY_ANY);
}


//! Affine tilemap demo
void test_tte_ase()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_DISPCNT= DCNT_MODE1 | DCNT_BG2;

	// Init affine text for 32x32t bg
	tte_init_ase(
		2,						// BG number
		BG_CBB(0) | BG_SBB(28) | BG_AFF_32x32,	// BG control
		0,						// Tile offset (special cattr)
		CLR_YELLOW,				// Ink color
		0xFE,					// BUP offset (on-pixel = 255)
		NULL,					// Default font (sys8)
		NULL);					// Default renderer (ase_drawg_s)

	// Write something
	tte_write("#{P:120,80}o");
	tte_write("#{P:72,104}Round, round, #{P:80,112}round we go");

	// Rotate it
	AFF_SRC_EX asx= { 124<<8, 84<<8, 120, 80, 0x100, 0x100, 0 };
	bg_rotscale_ex(&REG_BG_AFFINE[2], &asx);

	while(1)
	{
		VBlankIntrWait();
		key_poll();

		asx.alpha += 0x111;
		bg_rotscale_ex(&REG_BG_AFFINE[2], &asx);

		if(key_hit(KEY_START))
			break;		
	}
}


//! Testing a bitmap renderer with JanoS' sub-pixel font.
void test_tte_bmp16()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

	tte_init_bmp(3, &yesh1Font, bmp16_drawg);
	tte_init_con();

	const char *str= 
	"http://en.wikipedia.org/wiki/Subpixel_rendering :\n"
	"Subpixel rendering is a way to increase the "
	"apparent \nresolution of a computer's liquid crystal "
	"display (LCD).\nIt takes advantage of the fact that "
	"each pixel on a color\nLCD is actually composed of "
	"individual red, green, and\nblue subpixel stripes to "
	"anti-alias text with greater\ndetail.\n\n"
	"  4x8 sub-pixel font by JanoS.\n"
	"  http://www.haluz.org/yesh/\n";

	tte_write(str);
	key_wait_till_hit(KEY_ANY);
}


//! Object demo
void test_tte_obj()
{
	// Base inits
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_DISPCNT= DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
	oam_init(oam_mem, 128);
	VBlankIntrWait();

	OBJ_ATTR *objs= &oam_mem[127];

	// Init object text, using verdana 9 (8x16 objects)
	tte_init_obj(
		objs,				// Start at back of OAM
		ATTR0_TALL,			// attr0: 8x16 objects
		ATTR1_SIZE_8,		// attr1: 8x16 objects
		0,					// attr2: nothing special
		CLR_YELLOW,			// Yellow ink
		0x0E,				// ink pixel 14+1 = 15
		&vwf_default,			// Verdana 9 font 
		NULL);				// Default renderer (obj_drawg)

	pal_obj_bank[1][15]= CLR_RED;

	// Write something (and prep for path)
	const char *str= "Parametrized object text, omg!!!";
	const int len= strlen(str);
	tte_write("Parametrized object text, #{cp:0x1000}omg#{cp:0}!!!");


	// Play with the objects
	int ii, t= 0x9000;
	while(1)
	{
		VBlankIntrWait();
		key_poll();

		// Make lissajous figure
		for(ii=0; ii<len; ii++)
		{
			int ti= t-ii*0x380;				// Get the path param for ii
			obj_set_pos(&objs[-ii], 
				(96*lu_cos(  ti)>>12)+120,	// y= Ay*cos(  t) + y0
				(64*lu_sin(2*ti)>>12)+80);	// x= Ax*sin(2*t) + x0
		}
		t += 0x00A0;

		if(key_hit(KEY_START))
			break;		
	}
}


//! Test chr4 renderer
void test_tte_chr4c()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	// Init for text
	tte_init_chr4c(
		0,							// BG number.
		BG_CBB(0)|BG_SBB(10),		// BG control.
		0xF000,						// Screen-entry offset
		bytes2word(13,15,0,0),		// Color attributes.
		CLR_BLACK,					// Ink color
		&verdana9_b4Font,			// Verdana 9, with shade.
		(fnDrawg)chr4c_drawg_b4cts_fast);	// b4cts renderer, asm version
	tte_init_con();					// Initialize console IO

	// Load map graphics
	LZ77UnCompVram(dungeon01Map, se_mem[12]);
	LZ77UnCompVram(dungeon01Tiles, tile_mem[2]);
	LZ77UnCompVram(dungeon01Pal, pal_bg_mem);

	// Copy text box and its palette.
	TSurface src, *dst= tte_get_surface();
	srf_init(&src, SRF_CHR4C, dlgboxTiles, 240, 32, 4, (u16*)dlgboxPal);
	srf_pal_copy(dst, &src, 16);
	schr4c_blit(dst, 0, 128, 240, 32, &src, 0, 0);

	// Create and print to a text box.
	win_textbox(0, 8, 160-32+4, 232, 160-4, 8);
	CSTR text=
		"#{P}Scroll with #{ci:1;cs:2}arrows#{ci:13;cs:15}, "
		"quit with #{ci:1;cs:2}start#{ci:13;cs:15}\n"
		"Box opacity with #{ci:3;cs:4}L/R#{ci:7;cs:9}";
	tte_write(text);

	// Reset margins for coord-printing
	tte_set_margins(8, 8, 232, 20);

	int x=128, y= 32, ey=8<<3;

	REG_BG2HOFS= x;
	REG_BG2VOFS= y;

	// Invisible map buildup!
	REG_BG2CNT= BG_CBB(2) | BG_SBB(12) | BG_REG_64x64;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG2 | DCNT_WIN0;

	while(1)
	{
		VBlankIntrWait();
		key_poll();
		
		// Scroll and blend
		x = clamp(x + key_tri_horz(), 0, 512+1-SCREEN_WIDTH);
		y = clamp(y + key_tri_vert(), 0, 512+1-SCREEN_HEIGHT);
		ey= clamp(ey+ key_tri_shoulder(), 0, 0x81);

		REG_BG2HOFS= x;
		REG_BG2VOFS= y;
		REG_BLDY= ey>>3;

		// Erase and print new position.
		tte_printf("#{es;P}%d, %d", x, y);
		
		if(key_hit(KEY_START))
			break;		
	}	
}


//! Test font use.
void test_fonts()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	tte_init_chr4c(0, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_YELLOW, NULL, NULL);
	tte_init_con();

	const TFont *fontTable[4]= 
	{
		&verdana9Font, &verdana9iFont, &verdana9bFont, &verdana10Font
	};

	tte_set_font_table(fontTable);
	pal_gradient_ex(pal_bg_mem, 1, 4, CLR_YELLOW, CLR_ORANGE);

	iprintf("#{P:12,12;f:0;ci:1}Thundercats#{w:30}");
	iprintf("#{P:24,36;f:1;ci:2}Thundercats!#{w:30}");
	iprintf("#{P:40,60;f:2;ci:3}Thundercats!!#{w:30}");
	iprintf("#{P:60,84;f:3;ci:4}HOOOOOOOOOOOO!!!");

	key_wait_till_hit(KEY_ANY);
}


//! Test VT sequences.
void test_vt()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_DISPCNT= 3 | DCNT_BG2 | DCNT_PAGE;

	tte_init_bmp(3, NULL, NULL);
	tte_init_con();
	
	iprintf("\033[24;30H Hello \033[8A t \033[1C h \033[1C a \033[1C r!");

	key_wait_till_hit(KEY_ANY);
}


//! chr4 slide show of TTE capabilities
void test_chr4c_slide()
{
	slide_show(0);
}

//! bmp16 slide show of TTE capabilities
void test_bmp16_slide()
{
	slide_show(1);
}

//! bmp8 slide show of TTE capabilities
void test_bmp8_slide()
{
	slide_show(2);
}


//! Set up a rectangle for text, with the non-text layers darkened for contrast. 
void win_textbox(int bgnr, int left, int top, int right, int bottom, int bldy)
{
	REG_WIN0H= left<<8 | right;
	REG_WIN0V=  top<<8 | bottom;
	REG_WIN0CNT= WIN_ALL | WIN_BLD;
	REG_WINOUTCNT= WIN_ALL;
	
	REG_BLDCNT= (BLD_ALL&~BIT(bgnr)) | BLD_BLACK;
	REG_BLDY= bldy;

	REG_DISPCNT |= DCNT_WIN0;

	tte_set_margins(left, top, right, bottom);
}

// --------------------------------------------------------------------
// Item list
// --------------------------------------------------------------------

const TMenuItem cUnits[]= 
{
	{
		"se4: 4bpp tilemap", 
		"TTE on regular tilemaps (4bit). \n"
		"Shows colors and font change.", 
		test_tte_se4
	},
	{
		"ase: affine tilemap", 
		"TTE on affine tilemaps.",
		test_tte_ase
	},
	{
		"bmp16: 16bpp subpixel font", 
		"Some mode 3 text using a 4x8 sub-pixel \nfont.", 
		test_tte_bmp16
	}, 
	{
		"obj: object text", 
		"Write a string using objects and play with \n"
		"it a little. The font is verdana 9 (vwf_default), \n"
		"illustrating non-8x8 glyph sizes.",
		test_tte_obj
	},
	{
		"chr4 : text in a box",
		"Tile-plotting over a text-box for in-game text", 
		test_tte_chr4c
	},
	{
		"TTE font use", 
		"Use different glyph-sets for italic, bold\n"
		"and size.", 
		test_fonts
	},
 	{
		"VT100 sequences", 
		"Use VT100 escape sequences for formatting\n",
		test_vt
	}, 
	{
		"TTE Profiling", 
		"Test the speed of the standard renderers.\n"
		"Press any key to go to the next renderer.\n",
		test_speeds
	},
	{
		"chr4: 4bpp tiles slide show", 
		"A slide show demonstrating the TTE \n"
		"capabilities for 4bit tiles (mode 0).", 
		test_chr4c_slide
	},
	{
		"bmp8: 8bpp bitmap slide show", 
		"A slide show demonstrating the TTE \n"
		"capabilities for 8bit bitmaps (mode 4).", 
		test_bmp8_slide
	},
	{
		"bmp16: 16bpp bitmap slide show", 
		"A slide show demonstrating the TTE \n"
		"capabilities for 16bit bitmaps (mode 3).", 
		test_bmp16_slide
	}
};

int main()
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	TMenu menu;
	menu_create(&menu, cUnits, countof(cUnits));
	
	while(1)
	{
		menu_init(&menu);
		menu_run(&menu);
		menu_act(&menu);
	}
}

// EOF
