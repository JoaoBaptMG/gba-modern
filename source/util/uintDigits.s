@--------------------------------------------------------------------------------
@ intDigits.s
@--------------------------------------------------------------------------------
@ Provides a function to quickly compute the decimal digits of an u32
@--------------------------------------------------------------------------------

@ char* uintDigits(char* buffer, u32 val);
@ buffer: r0, val: r1
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global uintDigits
    .type uintDigits STT_FUNC
uintDigits:
    @ This routine works by "dividing by 10" using a multiplicative
    @ factor of ceil(2^35/10)
    @ Load the "magic constant" (aka ceil(2^35/10))
    ldr     r12, =0xCCCCCCCD

.loop:
    umull   r2, r3, r12, r1     @ r2:r3 = r1 * ceil(2^35/10)
    mov     r3, r3, lsr #3      @ r3 = floor(r1 * ceil(2^35/10) / 2^35) = floor(r1/10)
    add     r2, r3, r3, lsl #2  @ r2 = r3*5 = floor(r1/10)*5
    cmp     r1, #9              @ check if the number has not yet totally consumed
    sub     r1, r1, r2, lsl #1  @ r1 = (r1 - floor(r1/10)*10) = r1 % 10
    strb    r1, [r0], #1        @ Augment the buffer
    bxls    lr                  @ Return if the number is zero already

    @ Manual unrolling, using the fact that r3 has the new value
    umull   r2, r1, r12, r3     @ r2:r1 = r3 * ceil(2^35/10)
    mov     r1, r1, lsr #3      @ r1 = floor(r3 * ceil(2^35/10) / 2^35) = floor(r3/10)
    add     r2, r1, r1, lsl #2  @ r2 = r1*5 = floor(r3/10)*5
    cmp     r3, #9              @ check if the number has not yet totally consumed
    sub     r3, r3, r2, lsl #1  @ r3 = (r3 - floor(r3/10)*10) = r3 % 10
    strb    r3, [r0], #1        @ Augment the buffer
    bxls    lr                  @ Return if the number is zero already
    b       .loop