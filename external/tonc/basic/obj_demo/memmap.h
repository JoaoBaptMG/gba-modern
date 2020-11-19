//
//  GBA Memory map 
//
//! \file tonc_memmap.h
//! \author J Vijn
//! \date 20060508 - 20060508
// 
// === NOTES ===
// * This is a _small_ set of typedefs, #defines and inlines that can 
//   be found in tonclib, and might not represent the 
//   final forms.

#ifndef __MEMMAP__
#define __MEMMAP__


// === MEMORY SECTIONS ================================================

// basic sections
#define MEM_IO		0x04000000
#define MEM_PAL		0x05000000	// no 8bit write !!
#define MEM_VRAM	0x06000000	// no 8bit write !!
#define MEM_OAM		0x07000000	// no 8bit write !!

// basic sizes
#define PAL_SIZE	0x00400
#define VRAM_SIZE	0x18000
#define OAM_SIZE	0x00400

// sub sizes
#define PAL_BG_SIZE		0x00200
#define PAL_OBJ_SIZE	0x00200
#define VRAM_BG_SIZE	0x10000
#define VRAM_OBJ_SIZE	0x08000
#define M3_SIZE			0x12C00
#define M4_SIZE			0x09600
#define M5_SIZE			0x0A000
#define VRAM_PAGE_SIZE	0x0A000

// sub sections
#define REG_BASE	MEM_IO

#define MEM_PAL_OBJ		(MEM_PAL + PAL_BG_SIZE)
#define MEM_VRAM_BACK	(MEM_VRAM+ VRAM_PAGE_SIZE)
#define MEM_VRAM_OBJ	(MEM_VRAM+ VRAM_BG_SIZE)


// === STRUCTURED MEMORY MAP ==========================================
// Heavily typedefed, watch your pointers
// Should simplify memory accesses


// --- PAL ---
// pal_bg_mem[y] = COLOR (color y)
#define pal_bg_mem		((COLOR*)MEM_PAL)
#define pal_obj_mem		((COLOR*)MEM_PAL_OBJ)

// pal_bg_bank[y] =	COLOR[] (bank y)
// pal_bg_bank[y][x] = COLOR (bank y, color x : color y*16+x)
#define pal_bg_bank		((PALBANK*)MEM_PAL)
#define pal_obj_bank	((PALBANK*)MEM_PAL_OBJ)


// --- VRAM ---
// tile_mem[y] = TILE[]   (char block y)
// tile_mem[y][x] = TILE (char block y, tile x)
#define tile_mem		( (CHARBLOCK*)MEM_VRAM)
#define tile8_mem		((CHARBLOCK8*)MEM_VRAM)

#define tile_mem_obj	( (CHARBLOCK*)MEM_VRAM_OBJ)
#define tile8_mem_obj	((CHARBLOCK8*)MEM_VRAM_OBJ)

// vid_mem[a] = COLOR
#define vid_mem			((COLOR*)MEM_VRAM)


// --- OAM ---
// oatr_mem[a] = OBJ_ATTR (oam entry a)
#define oam_mem			((OBJ_ATTR*)MEM_OAM)
#define obj_mem			((OBJ_ATTR*)MEM_OAM)
#define obj_aff_mem		((OBJ_AFFINE*)MEM_OAM)


// === REGISTER LIST ==================================================


// === VIDEO REGISTERS ===
#define REG_DISPCNT			*(vu32*)(REG_BASE+0x0000)	// display control
#define REG_DISPSTAT		*(vu16*)(REG_BASE+0x0004)	// display interupt status
#define REG_VCOUNT			*(vu16*)(REG_BASE+0x0006)	// vertical count

// === KEYPAD ===
#define REG_KEYINPUT		*(vu16*)(REG_BASE+0x0130)	// Key status
#define REG_KEYCNT			*(vu16*)(REG_BASE+0x0132)


#endif // __MEMMAP__
