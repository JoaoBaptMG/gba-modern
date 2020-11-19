//
// toolbox.h: 
// 
// Tools header for norf_demo
// 
// (20060211-20060922, cearn)
//
// === NOTES ===
// * This is a _small_ set of typedefs, #defines and inlines that can 
//   be found in tonclib, and might not represent the 
//   final forms.


#ifndef TOOLBOX_H
#define TOOLBOX_H

// === (tonc_types.h) ============================================

// --- primary typedefs ---

typedef unsigned char  u8,  byte;
typedef unsigned short u16, hword;
typedef unsigned int   u32, word;
typedef unsigned long long u64;

typedef signed char  s8;
typedef signed short s16; 
typedef signed int   s32;
typedef signed long long s64;

// and volatiles for registers 'n stuff
typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

// --- secondary typedefs ---

typedef u16 COLOR;
typedef u16 SCR_ENTRY, SE;
typedef struct { u32 data[8];  } TILE, TILE4;
typedef struct { u32 data[16]; } TILE8;


typedef SCR_ENTRY	SCREENBLOCK[1024];
typedef TILE		CHARBLOCK[512];
typedef TILE8		CHARBLOCK8[256];

// --- misc ---

#define INLINE static inline


// === (tonc_memmap.h) ===========================================

#define MEM_IO		0x04000000
#define MEM_PAL		0x05000000		// no 8bit write !!
#define MEM_VRAM	0x06000000		// no 8bit write !!

#define PAL_SIZE	0x00400
#define VRAM_SIZE	0x18000

#define M3_SIZE			0x12C00
#define M4_SIZE			0x09600
#define M5_SIZE			0x0A000
#define CBB_SIZE		0x04000
#define SBB_SIZE		0x00800
#define VRAM_BG_SIZE	0x10000


// --- memmap ---

// pal_bg_mem[y] = COLOR (color y)
#define pal_bg_mem		((COLOR*)MEM_PAL)

// --- VRAM ---
// tile_mem[y] = TILE[]   (char block y)
// tile_mem[y][x] = TILE (char block y, tile x)
#define tile_mem		( (CHARBLOCK*)MEM_VRAM)
#define tile8_mem		((CHARBLOCK8*)MEM_VRAM)

// se_mem[y] = SB_ENTRY[] (screen block y)
// se_mem[y][x] = screen entry (screen block y, screen entry x)
#define se_mem			((SCREENBLOCK*)MEM_VRAM)


// --- registers ------------------------------------------------------

#define REG_BASE	MEM_IO

#define REG_DISPCNT			*(vu32*)(REG_BASE+0x0000)	// display control
#define REG_DISPSTAT		*(vu16*)(REG_BASE+0x0004)	// display interupt status
#define REG_VCOUNT			*(vu16*)(REG_BASE+0x0006)	// vertical count

// --- background ---
#define REG_BG0CNT			*(vu16*)(REG_BASE+0x0008)	// bg 0-3 control
#define REG_BG1CNT			*(vu16*)(REG_BASE+0x000A)
#define REG_BG2CNT			*(vu16*)(REG_BASE+0x000C)
#define REG_BG3CNT			*(vu16*)(REG_BASE+0x000E)

#define REG_BG0HOFS			*(vu16*)(REG_BASE+0x0010)		// bg 0-3 origins
#define REG_BG0VOFS			*(vu16*)(REG_BASE+0x0012)
#define REG_BG1HOFS			*(vu16*)(REG_BASE+0x0014)
#define REG_BG1VOFS			*(vu16*)(REG_BASE+0x0016)
#define REG_BG2HOFS			*(vu16*)(REG_BASE+0x0018)
#define REG_BG2VOFS			*(vu16*)(REG_BASE+0x001A)
#define REG_BG3HOFS			*(vu16*)(REG_BASE+0x001C)
#define REG_BG3VOFS			*(vu16*)(REG_BASE+0x001E)

#define REG_KEYINPUT		*(vu16*)(REG_BASE+0x0130)	// Key status


// === (tonc_memdef.h) =======================================----

// --- REG_DISPCNT ---

#define DCNT_MODE0				 0	//!< Mode 0; bg 0-4: reg
#define DCNT_MODE1			0x0001	//!< Mode 1; bg 0-1: reg; bg 2: affine
#define DCNT_MODE2			0x0002	//!< Mode 2; bg 2-2: affine
#define DCNT_MODE3			0x0003	//!< Mode 3; bg2: 240x160\@16 bitmap
#define DCNT_MODE4			0x0004	//!< Mode 4; bg2: 240x160\@8 bitmap
#define DCNT_MODE5			0x0005	//!< Mode 5; bg2: 160x128\@16 bitmap
#define DCNT_GB				0x0008	//!< (R) GBC indicator
#define DCNT_PAGE			0x0010	//!< Page indicator
#define DCNT_OAM_HBL		0x0020	//!< Allow OAM updates in HBlank
#define DCNT_OBJ_2D				 0	//!< OBJ-VRAM as matrix
#define DCNT_OBJ_1D			0x0040	//!< OBJ-VRAM as array
#define DCNT_BLANK			0x0080	//!< Force screen blank
#define DCNT_BG0			0x0100	//!< Enable bg 0
#define DCNT_BG1			0x0200	//!< Enable bg 1
#define DCNT_BG2			0x0400	//!< Enable bg 2
#define DCNT_BG3			0x0800	//!< Enable bg 3
#define DCNT_OBJ			0x1000	//!< Enable objects
#define DCNT_WIN0			0x2000	//!< Enable window 0
#define DCNT_WIN1			0x4000	//!< Enable window 1
#define DCNT_WINOBJ			0x8000	//!< Enable object window


// --- REG_BGxCNT ---

#define BG_MOSAIC			0x0040	//!< Enable Mosaic
#define BG_4BPP					 0	//!< 4bpp (16 color) bg (no effect on affine bg)
#define BG_8BPP				0x0080	//!< 8bpp (256 color) bg (no effect on affine bg)
#define BG_WRAP				0x2000	//!< Wrap around edges of affine bgs
#define BG_SIZE0				 0
#define BG_SIZE1			0x4000
#define BG_SIZE2			0x8000
#define BG_SIZE3			0xC000
#define BG_REG_32x32			 0	//!< reg bg, 32x32 (256x256 px)
#define BG_REG_64x32		0x4000	//!< reg bg, 64x32 (512x256 px)
#define BG_REG_32x64		0x8000	//!< reg bg, 32x64 (256x512 px)
#define BG_REG_64x64		0xC000	//!< reg bg, 64x64 (512x512 px)
#define BG_AFF_16x16			 0	//!< affine bg, 16x16 (128x128 px)
#define BG_AFF_32x32		0x4000	//!< affine bg, 32x32 (256x256 px)
#define BG_AFF_64x64		0x8000	//!< affine bg, 64x64 (512x512 px)
#define BG_AFF_128x128		0xC000	//!< affine bg, 128x128 (1024x1024 px)

#define BG_PRIO_MASK	0x0003
#define BG_PRIO_SHIFT		0
#define BG_PRIO(n)		((n)<<BG_PRIO_SHIFT)

#define BG_CBB_MASK		0x000C
#define BG_CBB_SHIFT		 2
#define BG_CBB(n)		((n)<<BG_CBB_SHIFT)

#define BG_SBB_MASK		0x1F00
#define BG_SBB_SHIFT		 8
#define BG_SBB(n)		((n)<<BG_SBB_SHIFT)

#define BG_SIZE_MASK	0xC000
#define BG_SIZE_SHIFT		14
#define BG_SIZE(n)		((n)<<BG_SIZE_SHIFT)


#define BG_BUILD(cbb, sbb, size, bpp, prio, mos, wrap)		\
(															\
	   ((size)<<14)  | (((wrap)&1)<<13) | (((sbb)&31)<<8	\
	| (((bpp)&8)<<4) | (((mos)&1)<<6)   | (((cbb)&3)<<2)	\
	| ((prio)&3)											\
)

// --- REG_KEYINPUT ---

#define KEY_A			0x0001	//!< Button A
#define KEY_B			0x0002	//!< Button B
#define KEY_SELECT		0x0004	//!< Select button
#define KEY_START		0x0008	//!< Start button
#define KEY_RIGHT		0x0010	//!< Right D-pad
#define KEY_LEFT		0x0020	//!< Left D-pad
#define KEY_UP			0x0040	//!< Up D-pad
#define KEY_DOWN		0x0080	//!< Down D-pad
#define KEY_R			0x0100	//!< Shoulder R
#define KEY_L			0x0200	//!< Shoulder L

#define KEY_ANY			0x03FF	//!< any key
#define KEY_DIR			0x00F0	//!< any-dpad
#define KEY_ACCEPT		0x0009	//!< A or start
#define KEY_CANCEL		0x0002	//!< B (well, it usually is)
#define KEY_SHOULDER	0x0300	//!< L or R

#define KEY_RESET		0x000F	//!< St+Se+A+B

#define KEY_MASK		0x03FF

// --- Reg screen entries ----------------------------------------------

#define SE_HFLIP		0x0400	//!< Horizontal flip
#define SE_VFLIP		0x0800	//!< Vertical flip

#define SE_ID_MASK		0x03FF
#define SE_ID_SHIFT			 0
#define SE_ID(n)		((n)<<SE_ID_SHIFT)

#define SE_FLIP_MASK	0x0C00
#define SE_FLIP_SHIFT		10
#define SE_FLIP(n)	 ((n)<<SE_FLIP_SHIFT)

#define SE_PALBANK_MASK		0xF000
#define SE_PALBANK_SHIFT		12
#define SE_PALBANK(n)		((n)<<SE_PALBANK_SHIFT)


#define SE_BUILD(id, pbank, hflip, vflip)	\
( ((id)&0x03FF) | (((hflip)&1)<<10) | (((vflip)&1)<<11) | ((pbank)<<12) )


// === (tonc_core.h) =============================================

// tribool: 1 if {plus} on, -1 if {minus} on, 0 if {plus}=={minus}
INLINE int bit_tribool(u32 x, int plus, int minus);


extern u16 __key_curr, __key_prev;


// === (tonc_video.h) ============================================

// --- sizes ---
#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	160

#define M3_WIDTH	SCREEN_WIDTH
#define M3_HEIGHT	SCREEN_HEIGHT
#define M4_WIDTH	SCREEN_WIDTH
#define M4_HEIGHT	SCREEN_HEIGHT
#define M5_WIDTH	160
#define M5_HEIGHT	128

// --- colors ---

#define CLR_BLACK	0x0000
#define CLR_RED		0x001F
#define CLR_LIME	0x03E0
#define CLR_YELLOW	0x03FF
#define CLR_BLUE	0x7C00
#define CLR_MAG		0x7C1F
#define CLR_CYAN	0x7FE0
#define CLR_WHITE	0x7FFF

INLINE COLOR RGB15(u32 red, u32 green, u32 blue);

INLINE void vid_vsync();


// === INLINES ========================================================

// --- (tonc_core.h) --------------------------------------------------

//! Gives a tribool (-1, 0, or +1) depending on the state of some bits.
/*! Looks at the \a plus and \a minus bits of \a flags, and subtracts 
*	  their status to give a +1, -1 or 0 result. Useful for direction flags.
*	\param plus		Bit number for positive result
*	\param minus	Bit number for negative result
*	\return	<b>+1</b> if \a plus bit is set but \a minus bit isn't<br>
*	  <b>-1</b> if \a minus bit is set and \a plus bit isn't</br>
*	  <b>0</b> if neither or both are set.
*/
INLINE int bit_tribool(u32 flags, int plus, int minus)
{	return ((flags>>plus)&1) - ((flags>>minus)&1);	}


// --- (tonc_video.h) -------------------------------------------------

//! Wait for next VBlank
INLINE void vid_vsync()
{
	while(REG_VCOUNT >= 160);   // wait till VDraw
	while(REG_VCOUNT < 160);    // wait till VBlank
}

//! Create a 15bit BGR color.
INLINE COLOR RGB15(u32 red, u32 green, u32 blue)
{	return red | (green<<5) | (blue<<10);	}



#endif // TOOLBOX_H

