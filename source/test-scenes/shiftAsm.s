@--------------------------------------------------------------------------------
@ shiftAsm.s
@--------------------------------------------------------------------------------
@ Provides functions with the sole purpose of testing the ASM-registers
@--------------------------------------------------------------------------------
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global lslAsm
    .type lslAsm STT_FUNC
lslAsm:
    mov     r0, r0, lsl r1
    bx      lr

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global lsrAsm
    .type lsrAsm STT_FUNC
lsrAsm:
    mov     r0, r0, lsr r1
    bx      lr