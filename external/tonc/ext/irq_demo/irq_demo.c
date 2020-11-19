//
// irq_demo.c
// Interrupts: routed, nested and prioritized
//
// (20061022 - 20080404, Cearn)
// === NOTES ===


#include <stdio.h>
#include <tonc.h>

IWRAM_CODE void isr_master();
IWRAM_CODE void hbl_grad_direct();

void vct_wait();
void vct_wait_nest();


CSTR strings[]= 
{
	"asm/nested",	"c/direct", 
	"VCount",		"HBlank"
};

// Function pointers to master isrs.
const fnptr master_isrs[2]= 
{
	(fnptr)isr_master,
	(fnptr)hbl_grad_direct 
};

// VCount interrupt routines.
const fnptr vct_isrs[2]= 
{
	vct_wait, 
	vct_wait_nest
};


// (1) Uses tonc_isr_master.s'  isr_master() as a switchboard
void hbl_grad_routed()
{
	u32 clr= REG_VCOUNT/8;
	pal_bg_mem[0]= RGB15(clr, 0, 31-clr);
}

// (2) VCT is triggered at line 80; this waits 40 scanlines
void vct_wait()
{
	pal_bg_mem[0]= CLR_RED;
	while(REG_VCOUNT<120);
}

// As vct_wait(), but interruptable by HBlank
void vct_wait_nest()
{
	pal_bg_mem[0]= CLR_RED;
	REG_IE= IRQ_HBLANK;		// Allow nested hblanks
	REG_IME= 1;
	while(REG_VCOUNT<120);
}


int main()
{
	u32 bDirect=0, bVctPrio= 0;

	tte_init_chr4c_b4_default(0, BG_CBB(2)|BG_SBB(28));
	tte_set_drawg((fnDrawg)chr4c_drawg_b4cts_fast);
	tte_init_con();
	tte_set_margins(8, 8, 128, 64);

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	// (3) Initialise irqs; add HBL and VCT isrs 
	// and set VCT to trigger at 80
	irq_init(master_isrs[0]);
	irq_add(II_HBLANK, hbl_grad_routed);
	BFN_SET(REG_DISPSTAT, 80, DSTAT_VCT);
	irq_add(II_VCOUNT, vct_wait);

	irq_add(II_VBLANK, NULL);

	while(1)
	{
		//vid_vsync();
		VBlankIntrWait();
		key_poll();

		// Toggle HBlank irq
		if(key_hit(KEY_R))
			REG_IE ^= IRQ_HBLANK;

		// Toggle Vcount irq
		if(key_hit(KEY_L))
			REG_IE ^= IRQ_VCOUNT;

		// (4) Toggle between 
		// asm switchblock + hbl_gradient (red, descending)
		// or purely hbl_isr_in_c (green, ascending)
		if(key_hit(KEY_A))
		{
			bDirect ^= 1;
			irq_set_master(master_isrs[bDirect]);
		}

		// (5) Switch priorities of HBlank and VCount
		if(key_hit(KEY_B))
		{
			//irq_set(II_VCOUNT, vct_wait, bVctPrio);
			bVctPrio ^= 1;
			irq_add(II_VCOUNT, vct_isrs[bVctPrio]);
		}

		tte_printf("#{es;P}ISR#{X:32}: %s\nPrio#{X:32}: %s\nIE#{X:32}: %04X", 
			strings[bDirect], strings[2+bVctPrio], REG_IE);
	}

	return 0;
}
