@--------------------------------------------------------------------------------
@ sqrt.h
@--------------------------------------------------------------------------------
@ Provides an implementation of a square root algorithm for 32-bits
@--------------------------------------------------------------------------------

@ Source code taken from http://www.pertinentdetail.org/sqrt
@ The actual function
@ u32 isqrt32(u32 x) // so x is in r0
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global isqrt32
    .type isqrt32 STT_FUNC
isqrt32:
    @ Registers: r0 = root, r1 = x, r2 = offset
    mov     r1, r0             @ move x to r1 first
    mov     r2, #3 << 30       @ initialize offset
    mov     r0, #1 << 30       @ initialize the first root

    @ now, 16 times, we will do the "simulation" loop
    cmp     r1, r0
    subhs   r1, r1, r0
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #2
    subhs   r1, r1, r0, ror #2
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #4
    subhs   r1, r1, r0, ror #4
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #6
    subhs   r1, r1, r0, ror #6
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #8
    subhs   r1, r1, r0, ror #8
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #10
    subhs   r1, r1, r0, ror #10
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #12
    subhs   r1, r1, r0, ror #12
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #14
    subhs   r1, r1, r0, ror #14
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #16
    subhs   r1, r1, r0, ror #16
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #18
    subhs   r1, r1, r0, ror #18
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #20
    subhs   r1, r1, r0, ror #20
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #22
    subhs   r1, r1, r0, ror #22
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #24
    subhs   r1, r1, r0, ror #24
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #26
    subhs   r1, r1, r0, ror #26
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #28
    subhs   r1, r1, r0, ror #28
    adc     r0, r2, r0, lsl #1

    cmp     r1, r0, ror #30
    subhs   r1, r1, r0, ror #30
    adc     r0, r2, r0, lsl #1

    bic r0, r0, #3 << 30
    bx lr
