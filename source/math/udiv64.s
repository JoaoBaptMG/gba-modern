@--------------------------------------------------------------------------------
@ udiv64.s
@--------------------------------------------------------------------------------
@ Provides an implementation of 64-bit/64-bit unsigned division
@--------------------------------------------------------------------------------

@ Source code adapted from the 32-bit/32-bit routine to work with 64-bit numerators
@ Original source code in https://www.chiark.greenend.org.uk/~theom/riscos/docs/ultimate/a252div.txt
@ r0:r1: the numerator / r2:r3: the denominator
@ after it, r0:r1 has the quotient and r2:r3 has the modulo
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_uldivmod
    .type __aeabi_uldivmod STT_FUNC
__aeabi_uldivmod:

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_uldiv
    .type __aeabi_uldiv STT_FUNC
__aeabi_uldiv:
    @ Check if the high word of the denominator is zero
    cmp     r3, #0
    beq     udiv64x32

    .global udiv64pastzero
udiv64pastzero:
    @ Check if the denominator is greater than the numerator and exit early if so
    cmp     r1, r3
    cmpeq   r0, r2
    blo     .zeroQuotient

    @ The fact that r3 != 0 gives us a nice optimization
    @ Since the quotient will then be 32-bit, we only need to use a
    @ 96-bit "numerator train", and it will finish nicely in 32 iterations
    push    {r4-r5}                 @ reserve space
    rsbs    r4, r2, #0              @ negate the denominator
    mov     r2, r1                  @ move the "numerator train" into place
    rsc     r1, r3, #0              @ negate with carry, to do the right task

    @ Now, we have r0:r2:r3 (in that order) is the "numerator train", with the
    @ remainder arriving in r2:r3 and the quotient in r0
    @ r4:r1 (in that order) is the denominator
    @ Build counter for optimization
    mov     r5, #30                 @ first guess on difference
    mov     r3, r2, lsr #2

    @ Iterate four times to get the counter up to 4-bit precision
    cmn     r1, r3, lsr #14     @ if denom <= (r1 >> 12)
    subge   r5, r5, #16         @ then -denom >= -(r1 >> 12)
    movge   r3, r3, lsr #16

    cmn     r1, r3, lsr #6
    subge   r5, r5, #8
    movge   r3, r3, lsr #8

    cmn     r1, r3, lsr #2
    subge   r5, r5, #4
    movge   r3, r3, lsr #4

    cmn     r1, r3
    subge   r5, r5, #2
    movge   r3, r3, lsr #2

    @ shift the rest of the numerator by the counter
    mov     r2, r2, lsl r5          @ r1 << r3
    rsb     r5, r5, #32
    orr     r2, r2, r0, lsr r5      @ r1 << r3 | (r0 >> (32-r3))
    rsb     r5, r5, #32
    mov     r0, r0, lsl r5          @ r0 << r3 - correctly set up
    adds    r0, r0, r0              @ bump r0 a first time

    @ dynamically jump to the exact copy of the iteration
    add     r5, r5, r5, lsl #3      @ multiply by 9
    add     pc, pc, r5, lsl #2      @ jump
    mov     r0, r0                  @ pipelining issues

    .global div64Iteration
div64Iteration:
    .rept 32                        @ any attempt at optimising those 9 instructions would be appreciated
    adcs    r2, r2, r2              @ (don't forget to update the multiplier up there, if you do manage it)
    adc     r3, r3, r3              @ this should be uncoupled in two additions like that
    adds    r2, r2, r4              @ because we can have a double-carry problem
    adcs    r3, r3, r1
    bcs     1f
    subs    r2, r2, r4
    sbc     r3, r3, r1
    adds    r0, r0, #0              @ this will clear the carry flag    
1:
    adcs    r0, r0, r0              @ so it can be correctly used here
    .endr

    @ from here, r0 = quotient, r2:r3 = remainder
    @ so it's just a matter of setting r1 = 0
    pop     {r4-r5}
    mov     r1, #0
    bx      lr

.zeroQuotient:
    @ n < d, so quot = 0 and rem = n
    mov     r2, r0
    mov     r3, r1
    mov     r0, #0
    mov     r1, #0
    bx      lr

@ r0: the number to get the reciprocal of
@ after it, r0:r1 = (1 << 64)/value
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global ureciprocal64
    .type ureciprocal64 STT_FUNC
ureciprocal64:
    @ check if r0 is 0 or 1, because it will overflow or divide by 0
    cmp     r1, #0
    cmpeq   r0, #1
    bls     __aeabi_idiv0

    @ move r0:r2:r3 = 1 << 64 and r4:r1 = -x
    push    {r4}
    rsbs    r4, r0, #0
    rsc     r1, r1, #0
    mov     r3, #1
    mov     r2, #0
    adds    r0, r2, r2        @ so r0 = 0, and clear the carry flag at the same time
    b       div64Iteration    @ the iteration is the same as for an unsigned division
