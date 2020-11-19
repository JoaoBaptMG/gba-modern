//
// m7_isrs.c
// Separate file for HBL interrupts because apparently it screws up 
//   on hardware now.

#include <tonc.h>

#ifndef __M7_DEMO__
#define __M7_DEMO__

// === CONSTANTS & MACROS =============================================

#define M7_D 160

enum eMode7Type
{	M7_BLOCK=0, M7_SAW, M7_SMOOTH};


// === GLOBALS ========================================================

extern VECTOR cam_pos;
extern u16 cam_phi;
extern FIXED g_cosf, g_sinf;
extern int g_state;

// === PROTOTYPES =====================================================

void set_mode7_type(int type);

IWRAM_CODE void m7_hbl_a();
IWRAM_CODE void m7_hbl_b();
IWRAM_CODE void m7_hbl_c();

#endif	// __M7_DEMO__
