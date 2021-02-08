@--------------------------------------------------------------------------------
@ shiftAsm.s
@--------------------------------------------------------------------------------
@ Provides test values for the overlay functions
@--------------------------------------------------------------------------------
    .section .iwram0, "ax", %progbits
    .align 2
    .arm
    .global overlayValue0
overlayValue0:
    .word   0xDEADBEF0

    .section .iwram1, "ax", %progbits
    .align 2
    .arm
    .global overlayValue1
overlayValue1:
    .word   0xDEADBEF1
