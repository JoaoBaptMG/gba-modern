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

@ context_t context_switch2(context_t ctx, context_t* outCtx)
@ switches to the context ctx returning the original context on the other side
@ and also on *r1
@ ctx: r0, outCtx: r1
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
    eor     r0, sp, r0      @ XOR-swap r0 and the stack pointer
    eor     sp, r0, sp
    eor     r0, sp, r0
    pop     {r4-r11, lr}    @ Restore the static registers and branch
    str     r0, [r1]        @ Store the r0 somewhere
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
    mov     sp, r0          @ null out the context pointer
    mov     r0, #0
    pop     {r4-r11, lr}    @ Restore the static registers and branch
    bx      lr

#include "context.h"

@ context_t context_switch_irq(context_t ctx)
@ besides context switching, also copies a portion of the IRQ stack
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global context_switch_irq
    .type   context_switch_irq STT_FUNC
context_switch_irq:
    @ Test first if it is the null pointer or the same register
    cmp     r0, #0
    bxeq    lr
    cmp     r0, sp
    bxeq    lr

    push    {lr}                            @ push the LR
    sub     sp, sp, #CONTEXT_STACK_SIZE     @ create space for the IRQ stack
    adr     lr, .exitIrqContext             @ set the new lr
    push    {r4-r11, lr}                    @ push the "user" part of the context
    add     r12, sp, #36                    @ point at the bottom of the stack size

    mrs     r2, cpsr                        @ get the CPSR
    mov     r3, #0x12                       @ set IRQ mode
    msr     cpsr, r3

    @ Transfer enough words from the IRQ stack
    pop     {r4-r11}
    stmia   r12!, {r4-r11}

#if CONTEXT_STACK_SIZE == 40
    pop     {r4, r5}
    stmia   r12!, {r4, r5}
#elif CONTEXT_STACK_SIZE == 36
    pop     {r4}
    stmia   r12!, {r4}
#else
#error This stack size is not recognized! Maybe you want to implement the copying manually?
#endif

    msr     cpsr, r2        @ go back to the previous mode
    eor     r0, sp, r0
    eor     sp, r0, sp
    eor     r0, sp, r0
    pop     {r4-r11, lr}    @ Restore the static registers and branch
    bx      lr

.exitIrqContext:
    add     r12, sp, #CONTEXT_STACK_SIZE    @ store the user-mode stack pointer
    mrs     r2, cpsr                        @ get the CPSR
    mov     r3, #0x12                       @ set IRQ mode
    msr     cpsr, r3

    @ Transfer enough words back to the IRQ stack
    ldmdb   r12!, {r4-r11}
    push    {r4-r11}

#if CONTEXT_STACK_SIZE == 40
    ldmdb   r12!, {r4, r5}
    push    {r4, r5}
#elif CONTEXT_STACK_SIZE == 36
    ldmdb   r12!, {r4}
    push    {r4}
#else
#error This stack size is not recognized! Maybe you want to implement the copying manually?
#endif

    msr     cpsr, r2        @ go back to the previous mode
    sub     sp, sp, #36     @ get the stored stack
    pop     {r4-r11}        @ restore the registers (all over again)
    add     sp, sp, #CONTEXT_STACK_SIZE+4
    pop     {lr}            @ finally pop the lr
    bx      lr
