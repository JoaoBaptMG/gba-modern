@--------------------------------------------------------------------------------
@ context.s
@--------------------------------------------------------------------------------
@ Provides an API for context switching (a primitive form of coroutines)
@--------------------------------------------------------------------------------

@ context_t context_switch(context_t ctx)
@ switches to the context ctx returning the original context on the other side
@ ctx: r0
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global context_switch
    .type   context_switch STT_FUNC
context_switch:
    @ Test first if it is the null pointer or the same register
    cmp     r0, #0
    bxeq    lr
    cmp     r0, sp
    bxeq    lr

    push    {r4-r11, lr}    @ Save the static registers to stack
    eor     r0, sp, r0      @ XOR-swap r0 and the stack pointer
    eor     sp, r0, sp
    eor     r0, sp, r0
    pop     {r4-r11, lr}    @ Restore the static registers and branch
    bx      lr              @ popping to pc doesn't work because the T flag is not updated (GBA only)

@ context_t context_switch2(context_t ctx, context_t* oldCtx)
@ does the same as context_switch, but returns oldCtx in r1
@ ctx: r0, oldCtx: r1
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global context_switch2
    .type   context_switch2 STT_FUNC
context_switch2:
    @ Test first if it is the null pointer or the same register
    cmp     r0, #0
    bxeq    lr
    cmp     r0, sp
    bxeq    lr

    push    {r4-r11, lr}    @ Save the static registers to stack
    str     sp, [r1]        @ store it into *r1
    eor     r0, sp, r0      @ XOR-swap r0 and the stack pointer
    eor     sp, r0, sp
    eor     r0, sp, r0
    pop     {r4-r11, lr}    @ Restore the static registers and branch
    bx      lr              @ popping to pc doesn't work because the T flag is not updated (GBA only)

@ ctx_entry_point pops the real entry_point and the first arg from stack
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global ctx_entry_point
    .type   ctx_entry_point STT_FUNC
ctx_entry_point:
    pop     {r1, r2}        @ get the entry point for the context
    mov     lr, pc          @ "emulate" a bl
    bx      r2              @ jump to the entry point
    mov     sp, r0          @ move the current context
    mov     r0, #0          @ null the context pointer
    pop     {r4-r11, lr}    @ Restore the static registers and branch
    bx      lr

@ for LR_STACK_OFFSET
#include "context.h"

@ void context_switch_from_irq(context_t ctx)
@ modifies the lr of the ISR so it behaves as if it's doing a context switch
@ and modifies the lr of the current context so it emulates a return from IRQ
@ r0: ctx
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global context_switch_from_irq
    .type   context_switch_from_irq STT_FUNC
context_switch_from_irq:
    @ quick sanity check
    cmp     r0, #0
    bxeq    lr
    cmp     r0, sp
    bxeq    lr

    @ from that point, assuming that the normal interrupt switchboards from Tonc and/or libgba are used
    @ the sp_irq stack should look like this: spsr IME (something) lr_irq r0 r1 r2 r3 r12 lr_ret
    @ except in the case of nesting Tonc subroutine, where the stack is one register smaller (no IME)
    @ so we need to add 40 (36 in the case of the nesting subroutine) to get to the IRQ stack
    @ we need to get r0 r1 r2 r3 r12 lr_ret and spsr in place

    @ Pick up sp_irq and store it in r12
    mrs     r3, cpsr                @ get the cpsr
    bic     r1, r3, #0xDF           @ clear the mode bits
    orr     r1, r1, #0x92           @ set IRQ mode
    msr     cpsr, r1                @ set back the CPSR
    add     r12, sp, #STACK_SIZE    @ now r12 points to LR_IRQ (and thus to the bottom of the IRQ stack)
    mrs     r1, spsr                @ set the spsr to this IRQ's spsr
    msr     cpsr, r3                @ go back to user mode

    @ Now for some tricky stack manipulation
    push    {r4-r11, lr}            @ push all the usual registers to sp
    ldmdb   r12, {r4-r9}            @ get all registers from the IRQ stack
    push    {r1-r9}                 @ and push the IRQ stack + spsr
    @ (the stack should be now spsr [sp_sys] [lr_sys] r0 r1 r2 r3 r12 lr_ret)

    @ Now we need to alter the IRQ stack so it returns to a "context switch"
    adr     r3, .irqCtx+4           @ get the context switch address (why +4? because the BIOS calls subs pc, lr, #4)
    stmfd   r12, {r0, r3}           @ set r0 (aka the context to switch) to the stack's r12
                                    @ and its lr to the the irq sym
    bic     r1, #0x20               @ clear the Thumb bit for spsr
    str     r1, [r12, #-STACK_SIZE] @ and store it back to its place

    @ Finally, we "hijack" this context lr so we can return to the exact point that we need
    add     sp, sp, #4
    str     sp, .irqTmpPtr          @ store it on the static address
    adr     r3, .irqReturnSim       @ set the simulator return routine
    str     r3, [sp, #64]!          @ set the return address for the irqSim
    add     sp, sp, #4

    @ Return to the calling function - this might cause stack thrashing if this function is not the last one to be called
    bx      lr

@ the piece of code that will switch the context after IRQ return
.irqTmpPtr:
    .word   0               @ static address to the switched to context

.irqCtx:
    ldr     r0, .irqTmpPtr  @ load the "hijacked" context (not yet!) to return
    stmia   r0!, {sp, lr}   @ we need to pick up the sp and the lr here, so they are returned correctly
    add     r0, r0, #24     @ adjust the r0 
    mov     sp, r12         @ load the context to switch to sp
    pop     {r4-r11, lr}    @ pop all the main registers
    bx      lr              @ and return

.irqReturnSim:
    @ this is something critical, as it emulates the BIOS
    @ stack should now be spsr sp_sys lr_sys r0 r1 r2 r3 r12 lr_ret (9 registers of garbage)
    sub     r0, sp, #72             @ point sp to the bottom of the stack
    ldmia   r0!, {r1, sp, lr}       @ load the three needed pointers
    mov     r3, #0x12               @ set the processor to interrupt mode
    msr     cpsr, r3
    msr     spsr, r1                @ move the spsr to its place
    ldmia   r0, {r0-r3, r12, lr}    @ load the saved registers
    subs    pc, lr, #4              @ return to the function, CPSR = SPSR
