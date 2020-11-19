//
// toolbox.h
// 
// Tools header for m3_demo
// 
// (20061004-20061004, cearn)
//
// === NOTES ===
// * This is a _small_ set of typedefs, #defines and inlines that can 
//   be found in tonclib, and might not represent the 
//   final forms.


#ifndef TOOLBOX_H
#define TOOLBOX_H

// === (tonc_types.h) ============================================

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

typedef u16 COLOR;

#define INLINE static inline

// === (tonc_memmap.h) ===========================================

#define MEM_IO		0x04000000
#define MEM_PAL		0x05000000		// no 8bit write !!
#define MEM_VRAM	0x06000000		// no 8bit write !!

#define PAL_SIZE		0x00400
#define VRAM_SIZE		0x18000

#define M3_SIZE			0x12C00


// --- memmap ---

// pal_bg_mem[y] = COLOR (color y)
#define pal_bg_mem		((COLOR*)MEM_PAL)

// vid_mem[a] = COLOR
#define vid_mem			((COLOR*)MEM_VRAM)


#define REG_BASE	MEM_IO

#define REG_DISPCNT			*(vu32*)(REG_BASE+0x0000)	// display control
#define REG_DISPSTAT		*(vu16*)(REG_BASE+0x0004)	// display interupt status
#define REG_VCOUNT			*(vu16*)(REG_BASE+0x0006)	// vertical count


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

#define DCNT_MODE_MASK		0x0007
#define DCNT_MODE_SHIFT			 0
#define DCNT_MODE(n)	   ((n)<<DCNT_MODE_SHIFT)

#define DCNT_LAYER_MASK		0x1F00
#define DCNT_LAYER_SHIFT		 8
#define DCNT_LAYER(n)	  ((n)<<DCNT_LAYER_SHIFT)

#define DCNT_WIN_MASK		0xE000
#define DCNT_WIN_SHIFT			13
#define DCNT_WIN(n)			((n)<<DCNT_WIN_SHIFT)

#define DCNT_BUILD(mode, layer, win, obj1d, objhbl)				\
(																\
		(((win)&7)<<13) | (((layer)&31)<<8) | (((obj1d)&1)<<6)	\
	| (((oamhbl)&1)<<5) | ((mode)&7)							\
)


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

INLINE void vid_vsync();
INLINE COLOR RGB15(u32 red, u32 green, u32 blue);

INLINE void m3_plot(int x, int y, COLOR clr);
INLINE void m3_line(int x1, int y1, int x2, int y2, COLOR clr);
INLINE void m3_rect(int left, int top, int right, int bottom, COLOR clr);
INLINE void m3_frame(int left, int top, int right, int bottom, COLOR clr);
void m3_fill(COLOR clr);


// 16bit (mode 3/5)
void bmp16_line(int x1, int y1, int x2, int y2, u32 clr, 
	void *dstBase, u32 dstPitch);
void bmp16_rect(int left, int top, int right, int bottom, u32 clr,
	void *dstBase, u32 dstPitch);
void bmp16_frame(int left, int top, int right, int bottom, u32 clr,
	void *dstBase, u32 dstPitch);


// === INLINES ========================================================


// --- (tonc_video.h) -------------------------------------------------

//! Wait for next VBlank
INLINE void vid_vsync()
{
	while(REG_VCOUNT >= 160);	// wait till VDraw
	while(REG_VCOUNT < 160);	// wait till VBlank
}

//! Create a 15bit BGR color.
INLINE COLOR RGB15(u32 red, u32 green, u32 blue)
{	return red | (green<<5) | (blue<<10);	}


// --- mode 3 interface ---

//! Plot a single \a clr colored pixel in mode 3 at (\a x, \a y).
INLINE void m3_plot(int x, int y, COLOR clr)
{
	vid_mem[y*M3_WIDTH+x]= clr;	
}


//! Draw a \a clr colored line in mode 3.
INLINE void m3_line(int x1, int y1, int x2, int y2, COLOR clr)
{
	bmp16_line(x1, y1, x2, y2, clr, vid_mem, M3_WIDTH*2);
}


//! Draw a \a clr colored rectangle in mode 3.
/*! \param ll	Left side, inclusive.
*	\param tt	Top size, inclusive.
*	\param rr	Right size, exclusive.
*	\param bb	Bottom size, exclusive.
*	\param clr	Color.
*/
INLINE void m3_rect(int left, int top, int right, int bottom, COLOR clr)
{
	bmp16_rect(left, top, right, bottom, clr, vid_mem, M3_WIDTH*2);
}


//! Draw a \a clr colored frame in mode 3.
/*! \param ll	Left side, inclusive.
*	\param tt	Top size, inclusive.
*	\param rr	Right size, exclusive.
*	\param bb	Bottom size, exclusive.
*	\param clr	Color.
*/
INLINE void m3_frame(int left, int top, int right, int bottom, COLOR clr)
{
	bmp16_frame(left, top, right, bottom, clr, vid_mem, M3_WIDTH*2);
}


#endif // TOOLBOX_H

