@--------------------------------------------------------------------------------
@ saveSelectionHblankIrq.s
@--------------------------------------------------------------------------------
@ Provides the required vblank to the player
@--------------------------------------------------------------------------------
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global saveSelectionHblankIrq
    .type saveSelectionHblankIrq STT_FUNC
saveSelectionHblankIrq:
    push    {r4-r5}
    @ since in a IRQ r0 starts at 0x4000200, adjust it
    sub     r0, r0, #0x1E0      @ now r0 points at 0x4000020 (aka REG_BG2PA)
    ldrh    r1, [r0, #-0x1A]    @ load vcount into r1
    cmp     r1, #227            @ compare it to the last vcount value
    moveq   r1, #0              @ and set it to zero if this is the case
    addne   r1, r1, #1          @ we need to load for the NEXT line, so add 1
    add     r1, r1, r1, lsl #1  @ multiply by 3
    ldr     r2, curHblankFrame  @ load the current frame to memory
    add     r2, r2, r1, lsl #2  @ load the current line entry ((VCOUNT+1)*12)
    ldmia   r2, {r3-r5}         @ load the data lines
    strh    r5, [r0], #8        @ load BG2PA into its place
    stmia   r0, {r3-r4}         @ load BG2X and BG2Y into their places
    movs    r5, r5, lsr #16     @ shift the data left to get the bgcnt
    strneh  r5, [r0, #-28]      @ store BG2CNT if it's nonzero
    pop     {r4-r5}
    bx      lr

    .global curHblankFrame
curHblankFrame:                 
    .word   0                   @ space for the current frame
