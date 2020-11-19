//
// Slide-show for TTE demonstration
//
//! \file 
//! \author J Vijn
//! \date 20070903 - 20070903
//
/* === NOTES ===
*/

#include <limits.h>
#include <tonc.h>
#include "verdana11.h"


// --------------------------------------------------------------------
// CONSTANTS 
// --------------------------------------------------------------------

enum ESlideStringIDs 
{
	SLIDE_FONT_STD=0, SLIDE_FONT_I, SLIDE_FONT_B, SLIDE_FONT_H1, 
	SLIDE_WAIT0, SLIDE_WAIT1, SLIDE_WAIT2, 
	SLIDE_PAGE,
	SLIDE_PAPER_STD, SLIDE_PAPER1, SLIDE_PAPER2, SLIDE_PAPER3, SLIDE_PAPER4, 
	SLIDE_INK_STD, SLIDE_INK_KEYW, SLIDE_INK_CMD, SLIDE_INK_FTR, SLIDE_INK_H1, 
	SLIDE_STR_MAX
};

// --------------------------------------------------------------------
// CLASSES 
// --------------------------------------------------------------------

typedef struct TSlide
{
	const char *header;
	CSTR *strTable;
	int strCount;
} TSlide;

// --------------------------------------------------------------------
// GLOBALS 
// --------------------------------------------------------------------

COLOR cSlidePal[16]= 
{
	0x1084, 0x28E7, 0x454A, 0x61AD, 0x7E10,      0, 0, 0,
	0x43FF, 0x0218, 0x7E1F, 0x6F7B, 0x7E10, 0x0280, 0, 0
};


char gStringBuffer[32];		// For page-number variable (and others >_>)

CSTR cSlideClrid[]= 
{
	"#{cp:0}", "#{cp:1}", "#{cp:2}", "#{cp:3}", "#{cp:4}", 
	"#{ci:8}", "#{ci:9}", "#{ci:10}", "#{ci:11}","#{ci:12}", 
};

CSTR cSlideRGB[]= 
{
	"#{cp:0x1084}", "#{cp:0x28E7}", "#{cp:0x454A}", "#{cp:0x61AD}", "#{cp:0x7E10}", 
	"#{ci:0x43FF}", "#{ci:0x0218}", "#{ci:0x7E1F}", "#{ci:0x6F7B}", "#{ci:0x7E10}", 
};


const char *gSlideStrings[SLIDE_STR_MAX]= 
{
	"#{f:0}", "#{f:1}", "#{f:2}", "#{f:3}", 
	"#{w:5}", "#{w:30}", "#{w:60}", gStringBuffer, 
	// 10 empty for colors
};

const TFont *cSlideFonts[4]= 
{
	&verdana9Font, &verdana9iFont, &verdana9bFont, &verdana11Font
};

// Slide strings

/*
#{ms;P;f:0;s:8;s:13}
	#{f:3;s:17}Menu#{f:0;s:13}
	Choose render type
	
		\t#{s:14}ase#{s:13} : Affine tilemaps\n
		\t#{s:14}bmp16#{s:13} : 16bpp bitmaps\n
		\t#{s:14}bmp8#{s:13} : 8bpp bitmaps\n
		\t#{s:14}chr4#{s:13} : 4bpp tiles\n
		\t#{s:14}obj#{s:13} : objects\n
		\t#{s:14}se#{s:13} : Regular tilemaps\n
	
	<footer>(A ->)</footer>
</slide>
*/

CSTR cSlideText0[]= 
{
	"Scripting is done with '#{s:14}brace-controls#{s:13}':\n" ,
	"#{s:14}\\#{#{s:13}#{s:15}foo:bar;#{s:13}#{s:14}}#{s:13}.\n",
	"#{s:5}Controls include:\n",
	"\tAbsolute positioning (#{s:15}X/Y/P#{s:13})\n",
	"\tRelative positioning (#{s:15}x/y/p#{s:13})\n",
	"\tColors (#{s:15}c#{s:13})\n",
	"\tFonts (#{s:15}f#{s:13})\n",
	"\tMargins (#{s:15}m#{s:13})\n",
	"\tErasers (#{s:15}e#{s:13}) #{s:5}\n",
	"\tand frame-waits, as you can see (#{s:15}w#{s:13}) \n",
};

CSTR cSlideText1[]= 
{
	"Uppercase #{s:15}X:##{s:13}, #{s:15}Y:##{s:13} "
		"and #{s:15}P:#,##{s:13} control\n",
	"absolute positioning.#{s:5}",
	"#{P:32,48}#{s:15}P:32,48#{s:13}#{s:5}",
	"#{X:120}#{s:15}X:120#{s:13}#{s:5}",
	"#{Y:70}#{s:15}Y:70#{s:13}#{s:5}\n\n",
	"Without arguments, the cursor moves to the top \n",
	"and/or left of the page.\n#{s:6}",
	"You can save/restore positions with #{s:15}Ps#{s:13} and ",
	"#{s:15}Pr#{s:13}."
};

CSTR cSlideText2[]= 
{
	"Lowercase #{s:15}x:##{s:13}, #{s:15}y:##{s:13} "
		"and #{s:15}p:#,##{s:13} control\n", 
	"relative positioning.#{s:5}"
	"#{P:32,48}Start here.#{s:5}"
	"#{x:48}#{s:15}{x:48}#{s:13}#{s:5}",
	"#{y:22}#{s:15}{y:22}#{s:13}#{s:5}",
	"#{p:-160,-8}#{s:15}{p:-160,-8}#{s:13}#{s:5}"
};

CSTR cSlideText3[]= 
{
	"There are four ligh#{s:5}...#{s:6; el;P; s:5}",
	"There are four colors, used for different purposes#{s:5}\n",
	"\t#{s:14}ink#{s:13} (#{s:15}ci:##{s:13})\n",
	"\t#{s:14}shadow#{s:13} (#{s:15}cs:##{s:13})\n",
	"\t#{s:14}paper#{s:13} (#{s:15}cp:##{s:13})\n",
	"\t#{s:14}special#{s:13} (#{s:15}cx:##{s:13})\n",	
	"#{s:5}Usually, only ink and paper are relevant.#{s:5}\n\n",
	"Note that #{s:14}color#{s:13} can refer to a real color\n",
	"or palette index, depending on context."
};

CSTR cSlideText4[]= 
{
	"Using a #{s:14}font-table#{s:13}, one can select\n",
	"different fonts with #{s:15}f:##{s:13}. For example:\n#{s:5}",
	"\t#{f:0}Normal\n",
	"\t#{f:1}Italic\n",
	"\t#{f:2}Bold#{s:5}\n",
	"#{f:0}Be sure the renderer is able to handle the fonts,\n",
	"though. The default renderers should work for\n",
	"all sizes, as long as the glyph-formats are\n",
	"1bpp and in tile-strips."
};

CSTR cSlideText5[]= 
{
	"The #{s:14}margins#{s:13} indicate the active rectangle\n",
	"on screen, It is used for character wrapping and \n",
	"the erasers. The margins can be set with #{s:15}ml:##{s:13},\n", 
	"#{s:15}mt:##{s:13}, #{s:15}mr:##{s:13}, #{s:15}mb:##{s:13}, or ", 
	"#{s:15}m:#,#,#,##{s:13} for all.\n",
	"Erasing is done with #{s:15}es#{s:13}, #{s:15}el#{s:13}, ",
	"#{s:15}eb#{s:13}, #{s:15}ef#{s:13}.#{s:6}#{s:6}\n",
	"#{m:24,88,216,144;P}",
	"Set margins and clear with #{s:15}es#{s:13}.#{s:6}",
	"#{s:9;es}#{s:5}",
	"#{P}Clear complete line (#{s:15}el#{s:13})#{s:6}",
	"#{s:10;el}#{s:5}",
	"#{P}Clear line backward (#{s:15}eb#{s:13})#{s:6}",
	"#{s:11;eb}#{s:5}",
	"#{P}Clear line forward (#{s:15}ef#{s:13})#{s:6}",
	"#{s:12;ef}#{s:5}",
	"#{ms}",
};

const TSlide cSlides[]= 
{
	{ "TTE Scripting",		cSlideText0, countof(cSlideText0)	}, 
	{ "Absolute positions", cSlideText1, countof(cSlideText1)	},
	{ "Relative positions", cSlideText2, countof(cSlideText2)	},
	{ "Colors",				cSlideText3, countof(cSlideText3)	},
	{ "Fonts",				cSlideText4, countof(cSlideText4)	},
	{ "Margins/Erasing",	cSlideText5, countof(cSlideText5)	},
};


/*

</slide>

	#{f:3;s:17}Margins / Erasing#{f:0;s:13}

	[cp:PAPER0]

</slide>

[ erase footer; wait1; erase screen; wait1; ]

#{}
	[P:center]And that concludes our brief tour.

	#{s:6}

	<footer>(<- B  ([page])  A ->)</footer>
</slide>
*/

// --------------------------------------------------------------------
// FUNCTIONS 
// --------------------------------------------------------------------


int slide_run(const TSlide *slides, int id)
{
	int ii;
	const TSlide *sl= &slides[id];
	POINT16 pt;

	// Init: font/color/position
	tte_write("#{f:0; s:8;s:13; ms;es;P; s:4;}");

	// Header (centered)
	tte_write("#{f:3;s:17}");
	pt= tte_get_text_size(sl->header);
	iprintf("#{p:%d,2}%s#{f:0;s:13;ml:8;mt:16;P;s:5}", 
		(SCREEN_WIDTH-pt.x)/2,  sl->header);

	// Run through slide script
	for(ii=0; ii<sl->strCount; ii++)
	{
		tte_write(sl->strTable[ii]);
		//tte_write("#{s:4}");
	}

	// Display footer
	tte_write("#{s:6}");
	siprintf(gStringBuffer, "[%d]", id);
	pt= tte_get_text_size(gStringBuffer);
	iprintf("#{s:16;f:2; P:4,146}<- B#{X:%d;s:7;X:212}A ->", 
		(SCREEN_WIDTH-pt.x)/2);
	
	//# Wait for action (then wait a little longer)
	while(1)
	{
		VBlankIntrWait();
		key_poll();

		if(key_hit(KEY_LEFT|KEY_B))
			return id-1;
		else if(key_hit(KEY_RIGHT|KEY_A))
			return id+1;
		if(key_hit(KEY_START))
			return INT_MAX;
	}
	return id;
}


int slide_menu()
{
	memset32(vid_mem, 0, VRAM_BG_SIZE/4);

	// Set-up start screen (chr4)
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG2;
	tte_init_chr4c(2, BG_CBB(0)|BG_SBB(31), 0, 0x08, 0, NULL, 
		(fnDrawg)chr4c_drawg_b1cts_fast);
	tte_init_con();

	memcpy16(pal_bg_mem, cSlidePal, 16);

	CSTR menuStr[]= 
	{ 
		"chr4: 4bpp tiles (m0)", 
		"bmp16: 16bpp bitmap (m3)", 
		"bmp8: 8bpp bitmap (m4)" 
	}; 

	int ii, sel= 0;

	tte_write("#{cp:4;er:54,62,186,106; m:56,64,184,114;P; ci:9;cp:11}");
	for(ii=0; ii<3; ii++)
		iprintf("#{el} %s\n#{y:2}", menuStr[ii]);


	// Menu thingy
	while(1)
	{
		VBlankIntrWait();
		key_poll();

		tte_write("#{P}");
		for(ii=0; ii<3; ii++)
			iprintf("#{ci:%d} %s\n#{y:2}", sel==ii ? 13 : 9, menuStr[ii]);

		if(key_hit(KEY_DOWN|KEY_SELECT))
			sel++;
		else if(key_hit(KEY_UP))
			sel--;
		sel= wrap(sel, 0, 3);

		if(key_hit(KEY_ACCEPT))
			break;
	}

	// Blink selection; then clear
	iprintf("#{P;y:%d", (12+2)*sel);
	for(ii=0; ii<8; ii++)
	{
		iprintf("#{X;ci:%d} %s", ii&1 ? 9 : 13, menuStr[sel]);
		VBlankIntrDelay(10);
	}

	memset32(vid_mem, 0, VRAM_BG_SIZE/4);
	VBlankIntrDelay(30);

	return sel;
}

void slide_show(int mode)
{
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	int ii;

	// Set-up specific TTE stuff
	switch(mode)
	{
	case 0:			// chr4
		REG_DISPCNT= DCNT_MODE0 | DCNT_BG2;
		tte_init_chr4c(2, BG_CBB(0)|BG_SBB(31), 0, 0x08, 0, NULL, 
			(fnDrawg)chr4c_drawg_b1cts_fast);

		for(ii=0; ii<10; ii++)
			gSlideStrings[ii+8]= cSlideClrid[ii];
		
		break;
	case 1:			// bmp16
		REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;
		tte_init_bmp_default(3);

		for(ii=0; ii<10; ii++)
			gSlideStrings[ii+8]= cSlideRGB[ii];

		break;
	case 2:			// bmp8
		REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;
		vid_page= vid_mem;
		tte_init_bmp_default(4);

		for(ii=0; ii<10; ii++)
			gSlideStrings[ii+8]= cSlideClrid[ii];

		break;
	}

	tte_set_string_table(gSlideStrings);
	tte_set_font_table(cSlideFonts);

	memcpy16(pal_bg_mem, cSlidePal, 16);

	// Run slides
	ii= 0;
	while(1)
	{
		//# Run slides
		VBlankIntrDelay(5);
		ii= slide_run(cSlides, ii);
		if(ii == INT_MAX)
			return;
		ii= wrap(ii, 0, countof(cSlides));
	}
}

void slide_test()
{
	int mode;

	while(1)
	{
		mode= slide_menu();
		slide_show(mode);
	}
}


// EOF
