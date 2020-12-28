@--------------------------------------------------------------------------------
@ memswap32.h
@--------------------------------------------------------------------------------
@ Provides a function to swap two chunks of memory
@--------------------------------------------------------------------------------

@ void memswap32(void* mem1, void* mem2, u32 nwords)
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global memswap32
    .type   memswap32 STT_FUNC
memswap32:
    movs    r3, r2, lsr #2          @ check the number of 16-byte chunks
    beq     .finishCopying          @ if there's none, just go to the end of the copy

    push    {r4-r11}                @ allocate space for the scratch registers
.copyChunk:
    ldm     r0, {r4-r7}             @ load the first part
    ldm     r1, {r8-r11}            @ load the second part
    stm     r0!, {r8-r11}           @ store the second part in the first
    stm     r1!, {r4-r7}            @ store the first part in the second
    subs    r3, r3, #1              @ reduce the 16-byte count by one
    bne     .copyChunk              @ go back if there are still chunks to copy
    pop     {r4-r11}                @ pop the scratch registers back to the stack

.finishCopying:
    ands    r2, r2, #3              @ get the number of remaining copies after the main chunks
    bxeq    lr                      @ finish early if there are no copies to be done

.copySingle:
    ldr     r3, [r0]                @ load a single word
    swp     r3, r3, [r1]            @ store it to r1, load a word from r1
    str     r3, [r0], #4            @ store the other word from r1
    add     r1, r1, #4              @ update the pointer
    subs    r2, r2, #1              @ check the number of words to swap
    bne     .copySingle             @ go back if still there
    bx      lr                      @ return
