@--------------------------------------------------------------------------------
@ hblankReportCycles.s
@--------------------------------------------------------------------------------
@ Provides an ISR that uses a busy-loop to check for cycle availability
@--------------------------------------------------------------------------------
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global hblankReportCycles
    .type hblankReportCycles STT_FUNC
hblankReportCycles:
    mov     r0, #0x4000000      @ r0 = (&REG_DISPSTAT - 4)
    mov     r1, #0              @ the spin-lock counter (account for some overhead)

.spinLoop1:
    ldrh    r2, [r0, #4]        @ load REG_DISPSTAT
    tst     r2, #2              @ check the V-Blank flag
    beq     .spinLoop1          @ go back to the spin loop

.spinLoop2:
    ldrh    r2, [r0, #4]        @ load REG_DISPSTAT - 3 cycles
    ands    r2, r2, #2          @ check the V-Blank flag - 1 cycle
    addne   r1, #8              @ add if we're still in vblank - 1 cycle
    bne     .spinLoop2          @ go back to the spin loop - 3 cycles, total 8

    add     r0, r0, #0x100
    strh    r2, [r0, #0xA]    @ stop timer 2 (r2 = 0 from the last part)

    .extern cycleCount
    ldr     r0, =cycleCount     @ store it on the address
    str     r1, [r0]

    bx      lr
