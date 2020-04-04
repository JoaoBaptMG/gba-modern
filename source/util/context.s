@--------------------------------------------------------------------------------
@ context.s
@--------------------------------------------------------------------------------
@ Provides an API for context switching (a primitive form of coroutines)
@--------------------------------------------------------------------------------

@ [context_t, void*] context_switch_asm(context_t ctx, void* arg)
@ switches to the context ctx, passing arg through it, returning the original
@ context on the other side
@ ctx: r0, arg: r1
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

.midCtx:
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
    mov     sp, #0          @ null out the context pointer
    b       .midCtx         @ jump to the end of the pointer
