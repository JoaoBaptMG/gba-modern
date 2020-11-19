//
// swi_vsync.c
// Vsyncing with BIOS call 5
//
// (Created: 20040602 - 20060928, Cearn)

#include <tonc.h>
#include "metr.h"

void init_main()
{
	// The obvious initialisations
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D;
	oam_init(oam_mem, 128);

	// Get the tiles & palette
	memcpy32(&tile_mem[4][0], metr_boxTiles, metr_boxTilesLen/4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen/4);

	// Initialize the metroid sprite
	OBJ_ATTR *metr= &oam_mem[0];
	obj_set_attr(metr, ATTR0_SQUARE | ATTR0_AFF_DBL,
		ATTR1_SIZE_64 | ATTR1_AFF_ID(0), 0);
	obj_set_pos(metr, 120-64, 80-64);
}

int main()
{
	AFF_SRC as= { 0x0100, 0x0100, 0 };
	OBJ_AFFINE oaff;

	init_main();

	// Init interrupts, and enable VBlank irq
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	while(1)
	{
		VBlankIntrWait();

		// Full circle = 10000h
		// 10000h/4/60= 111h -> 1/4 rev/s = 1 passing corner/s
		as.alpha += 0x0111;
		ObjAffineSet(&as, &oaff.pa, 1, OBJ_AFF_OFS);

		obj_aff_copy(obj_aff_mem, &oaff, 1);
	}

	return 0;
}
