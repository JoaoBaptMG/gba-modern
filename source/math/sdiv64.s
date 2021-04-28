@--------------------------------------------------------------------------------
@ sdiv64x32.s
@--------------------------------------------------------------------------------
@ Provides an implementation of 64-bit/64-bit signed division
@--------------------------------------------------------------------------------

@ r0:r1: the numerator / r2:r3: the denominator
@ after it, r0:r1 has the quotient and r2:r3 has the modulo
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_ldivmod
    .type __aeabi_ldivmod STT_FUNC
__aeabi_ldivmod:

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_ldiv
    .type __aeabi_ldiv STT_FUNC
__aeabi_ldiv:
    @ Test division by zero
    cmp     r3, #0
    cmpeq   r2, #0
    beq     __aeabi_idiv0

    @ Move the lr to r12 and make the numbers positive
    mov     r12, lr

    @ bit 28 is whether the numerator is negative
    cmp     r1, #0
    orrlt   r12, #1 << 28
    bge     .numeratorIsPositive
    rsbs    r0, r0, #0
    rsc     r1, r1, #0
.numeratorIsPositive:

    @ bit 31 is whether the denominator is negative
    cmp     r3, #0
    orrlt   r12, #1 << 31
    bge     .denominatorIsPositive
    rsbs    r2, r2, #0
    rsc     r3, r3, #0

.denominatorIsPositive:
    @ Call the unsigned division
    .extern udiv64pastzero
    bl      udiv64pastzero

    @ This moves "numerator is negative" to overflow flag and
    @ "denominator is negative" to sign flag
    msr     cpsr_f, r12

    @ The quotient should be negated only if exactly one (but not zero or two)
    @ of the numerator and/or denominator were negative, so that means N!=V
    @ that's why we use the lt condition here
    bge     .quotientShouldBePositive
    rsbs    r0, r0, #0
    rsc     r1, r1, #0
    msr     cpsr_f, r12     @ move the flags again
.quotientShouldBePositive:

    @ The modulo should only be negated if the numerator was negative, so if V=1
    bvc     .remainderShouldBePositive
    rsbs    r2, r2, #0
    rsc     r3, r3, #0
.remainderShouldBePositive:

    @ Erase the sign bits from the return address, and return
    bic     r12, #0xF << 28
    bx      r12

@ r0: the number to get the reciprocal of
@ after it, r0:r1 = (1 << 64)/value
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global sreciprocal32
    .type sreciprocal32 STT_FUNC
sreciprocal64:
    @ check if r0 is 0, 1 or -1, because it will overflow or divide by 0
    cmp     r1, #0
    bne     .isNotZero
    cmp     r0, #1
    bxls    lr          @ jump if x = 0 or 1
    cmn     r0, #1
    bxeq    lr          @ jump if x = 0xFFFFFFFF (aka -1)

.isNotZero:
    @ move lr to r12 and set its last flags if needed
    push    {r4}
    mov     r4, r0
    mov     r12, lr
    cmp     r1, #0
    orrlt   r12, #1 << 31
    blt     .isNegative

    @ set r0:r2:r3 = 1 << 64 and r4:r1 = -|x|
    rsbs    r4, r4, #0
    rsc     r1, r1, #0
.isNegative:
    mov     r3, #1
    mov     r2, #0
    adds    r0, r2, r2        @ so r0 = 0, and clear the carry flag at the same time
    bl      div64Iteration    @ the iteration is the same as for an unsigned division

    @ now, check to see if the result was negative before
    tst     r12, #1 << 31
    bxeq    r12

    @ negate the reciprocal
    rsbs    r0, r0, #0
    rsc     r1, r1, #0

    @ erase the flag and jump back
    bic     r12, #1 << 31
    bx      r12
