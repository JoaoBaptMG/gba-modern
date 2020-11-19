//
// isr.iwram.c
//
// (20050607 - 20050615, Cearn)
// === NOTES ===
// * An *.iwram.c is automatically put in IWRAM (in DKP at least)
//   Create an extra rule to compile as ARM code

#include <tonc.h>

IWRAM_CODE void hbl_grad_direct();

// an interrupt routine purely in C
// (make SURE you compile in ARM mode!!)
void hbl_grad_direct()
{
	u32 irqs= REG_IF & REG_IE;

	REG_IFBIOS |= irqs;

	if(irqs & IRQ_HBLANK)
	{
		u32 clr= REG_VCOUNT/8;
		pal_bg_mem[0]= RGB15(0, clr, 0);
	}

	REG_IF= irqs;
}


#define CPU_MODE_USER	(0x10<< 0)
#define CPU_MODE_FIQ	(0x11<< 0)
#define CPU_MODE_IRQ	(0x12<< 0)
#define CPU_MODE_SVC	(0x13<< 0)
#define CPU_MODE_ABT	(0x17<< 0)
#define CPU_MODE_UND	(0x1B<< 0)
#define CPU_MODE_SYS	(0x1F<< 0)

#define CPU_THUMB		(1<< 5)
#define CPU_FIQ_OFF		(1<< 6)
#define CPU_IRQ_OFF		(1<< 7)

#define CPU_MODE_MASK	(0x1F<< 0)


// This is mostly what tonclib's isr_master_nest does, but
// you really need asm for the full functionality
IWRAM_CODE void isr_master_nest_c()
{
	u32 ie= REG_IE, ieif= ie &REG_IF, irq_prio= 0;
	IRQ_REC *pir;

	REG_IFBIOS= ieif;	// Ack for BIOS routines

	// --- Find raised irq ---
	for(pir= __isr_table; pir->flag; pir++)
	{
		if(pir->flag & ieif)
			break;
		irq_prio |= pir->flag;
	}
	// Irq not recognized or has no isr:
	// Just ack in REG_IF and return
	if(pir->flag == 0 || pir->isr == NULL)
	{
		REG_IF= ieif;	
		return;
	}

	REG_IME= 0;

	// --- CPU back to system mode ---
	//> *--sp_irq= lr_irq;
	//> cpsr &= ~(CPU_MODE_MASK | CPU_IRQ_OFF);
	//> cpsr |= CPU_MODE_SYS;
	//> *--sp_sys = lr_sys;

	REG_IE= irq_prio;	// Allow only irqs of higher priority
	REG_IME= 1;

	pir->isr();

	u32 ime= REG_IME;
	REG_IME= 0;

	// --- Back to irq mode ---
	//> lr_sys = *sp_sys++;
	//> cpsr &= ~(CPU_MODE_MASK | CPU_IRQ_OFF);
	//> cpsr |= CPU_MODE_IRQ | CPU_IRQ_OFF;
	//> lr_irq = *sp_irq++;

	// Restore ie and ack handled irq
	REG_IE= ie;
	REG_IF= pir->flag<<16;
	REG_IME= ime;
}
