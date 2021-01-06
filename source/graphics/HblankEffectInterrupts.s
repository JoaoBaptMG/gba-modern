@--------------------------------------------------------------------------------
@ HblankEffects.cpp
@--------------------------------------------------------------------------------
@ Provides the class to manage the H-Blank effects
@--------------------------------------------------------------------------------

#define LOG_MAX_EFFECTS 4               @ log2 of the maximum number of effects

@ void updateEffectIsr();
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global updateEffectIsr
    .type updateEffectIsr STT_FUNC
updateEffectIsr:
    @ r0 is already 0x4000000 upon entering the ISR
    sub     r0, r0, #0x100
    mov     r1, #0                              @ zero out a register
    strh    r1, [r0, #0x6]                      @ stop timer 1

    push    {r4-r7}
    adr     r1, .addresses
    ldmia   r1, {r1-r3}                         @ load the addresses in the registers
    @ r1 = &hblankCurFrame, r2 = hblankLines, h3 = hblankWrites
    ldrh    r1, [r1]
    add     r2, r2, r1, lsl #4                  @ load the frame's flights
    add     r1, r1, r1, lsl #1                  @ multiply by 3
    add     r3, r3, r1, lsl #6                  @ multiply by (16)*12

    ldr     r1, =hblankCurIdx
    ldrh    r1, [r1]                            @ load the current index
    add     r3, r3, r1, lsl #2
    add     r3, r3, r1, lsl #3                  @ pre-load the index
    sub     r0, r0, #0x100 - 0xD4               @ REG_DMA3SAD
    ldrb    r7, [r2, r1]                        @ load the current line number
    add     r1, r1, #1                          @ pre-add the line number

    @ Now, wait until the HBlank flag kicks in
.waitForHBlank:
    ldrh    r4, [r0, #-0xD0]
    tst     r4, #2                              @ check for the Hblank tag
    beq     .waitForHBlank                      @ bail out if it haven't started yet

.nextVal:
    ldmia   r3!, {r4-r6}                        @ load the next line data
    stmia   r0, {r4-r6}                         @ and push it into DMA3
    ldrb    r4, [r2, r1]                        @ load the next line number
    cmp     r4, r7                              @ check if it is the same
    addeq   r1, r1, #1                          @ add one to it
    beq     .nextVal                            @ and go to the next one

    ldr     r7, =hblankCurIdx
    strh    r1, [r7]                            @ store again the current index
    cmp     r4, #0xff                           @ check if the next value is not a sentinel
    ldrh    r5, [r0, #-0xD0]!                   @ reload the dispstat register
    biceq   r5, r5, #4                          @ if no more lines, disable vcount interrupt
    bicne   r5, r5, #0xFF << 8                  @ clear the next vcount
    sub     r4, r4, #1
    orrne   r5, r5, r4, lsl #8                  @ put the new line count (minus one)
    strh    r5, [r0]                            @ store the value back
    
    @ finish
    pop     {r4-r7}
    bx      lr

.addresses:
    .extern     hblankCurFrame
    .extern     hblankLines
    .extern     hblankWrites
    .word       hblankCurFrame, hblankLines, hblankWrites
