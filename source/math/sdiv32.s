@--------------------------------------------------------------------------------
@ udiv.s
@--------------------------------------------------------------------------------
@ Provides an implementation of signed division
@--------------------------------------------------------------------------------

@ refer to the unsigned division
    .extern __aeabi_uidivmod
    .extern __aeabi_uidiv

@ r0: the numerator / r1: the denominator
@ after it, r0 has the quotient and r1 has the modulo
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_idivmod
    .type __aeabi_idivmod STT_FUNC
__aeabi_idivmod:

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_idiv
    .type __aeabi_idiv STT_FUNC
__aeabi_idiv:
    @ Test division by zero
    cmp     r1, #0
    beq     __aeabi_idiv0

    @ Move the lr to r12 and make the numbers positive
    mov     r12, lr

    @ bit 28 is whether the numerator is negative
    cmp     r0, #0
    rsblt   r0, #0
    orrlt   r12, #1 << 28

    @ bit 31 is whether the denominator is negative
    cmp     r1, #0
    rsblt   r1, #0
    orrlt   r12, #1 << 31

    @ Call the unsigned division
    .extern udiv32pastzero
    bl      udiv32pastzero

    @ This moves "numerator is negative" to overflow flag and
    @ "denominator is negative" to sign flag
    msr     cpsr_f, r12

    @ The quotient should be negated only if exactly one (but not zero or two)
    @ of the numerator and/or denominator were negative, so that means N!=V
    @ that's why we use the lt condition here
    rsblt   r0, r0, #0

    @ The modulo should only be negated if the numerator was negative, so if V=1
    rsbvs   r1, r1, #0

    @ Erase the sign bits from the return address, and return
    bic     r12, #0xF << 28
    bx      r12

@ r0: the number to get the reciprocal of
@ after it, r0 = (1 << 32)/value
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global sreciprocal32
    .type sreciprocal32 STT_FUNC
sreciprocal32:
    @ check if r0 is 0, 1 or -1, because it will overflow or divide by 0
    cmp     r0, #1
    bxls    lr          @ jump if x = 0 or 1
    cmn     r0, #1
    bxeq    lr          @ jump if x = 0xFFFFFFFF (aka -1)

    @ move lr to r12 and set its last flags if needed
    mov     r2, r0
    mov     r12, lr
    cmp     r0, #0
    orrlt   r12, #1 << 31

    @ set r0 = 0, r1 = 1 (so r0:r1 = 1 << 32) and r2 = -|x|
    rsbge   r2, r2, #0
    mov     r1, #1
    movs    r0, r1, lsr #32 @ so r0 = 0, and clear the carry flag at the same time
    bl      divIteration    @ the iteration is the same as for an unsigned division

    @ now, check to see if the result was negative before
    tst     r12, #1 << 31
    rsbne   r0, r0, #0

    @ erase the flag and jump back
    bic     r12, #1 << 31
    bx      r12
