@--------------------------------------------------------------------------------
@ udiv32.s
@--------------------------------------------------------------------------------
@ Provides an implementation of 32-bit/32-bit unsigned division
@--------------------------------------------------------------------------------

@ Source code taken from https://www.chiark.greenend.org.uk/~theom/riscos/docs/ultimate/a252div.txt
@ r0: the numerator / r1: the denominator
@ after it, r0 has the quotient and r1 has the modulo
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_uidivmod
    .type __aeabi_uidivmod STT_FUNC
__aeabi_uidivmod:

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global __aeabi_uidiv
    .type __aeabi_uidiv STT_FUNC
__aeabi_uidiv:

    @ Check for division by zero
    cmp     r1, #0
    beq     __aeabi_idiv0

    .global udiv32pastzero
udiv32pastzero:
    @ If n < d, just bail out as well
    cmp     r0, r1      @ n, d
    movlo   r1, r0    @ mod = n
    movlo   r0, #0    @ quot = 0
    bxlo    lr

    @ Move the denominator to r2 and start to build a counter that
    @ counts the difference on the number of bits on each numerator
    @ and denominator
    @ From now on: r0 = quot/num, r1 = mod, r2 = -denom, r3 = counter
    rsb     r2, r1, #0
    mov     r3, #28             @ first guess on difference
    mov     r1, r0, lsr #4      @ r1 = num >> 4

    @ Iterate three times to get the counter up to 4-bit precision
    cmn     r2, r1, lsr #12     @ if denom <= (r1 >> 12)
    subge   r3, r3, #16         @ then -denom >= -(r1 >> 12)
    movge   r1, r1, lsr #16

    cmn     r2, r1, lsr #4
    subge   r3, r3, #8
    movge   r1, r1, lsr #8

    cmn     r2, r1
    subge   r3, r3, #4
    movge   r1, r1, lsr #4

    @ shift the numerator by the counter
    mov     r0, r0, lsl r3
    adds    r0, r0, r0              @ bump r0 a first time

    @ dynamically jump to the exact copy of the iteration
    add     r3, r3, r3, lsl #1      @ counter *= 3
    add     pc, pc, r3, lsl #2      @ jump
    mov     r0, r0                  @ pipelining issues

    @ here, r0 = num << (r3 + 1), r1 = num >> (32-r3), r2 = -denom
    @ now, the real iteration part
    .global div32Iteration
div32Iteration:
    .rept 32
    adcs    r1, r2, r1, lsl #1
    subcc   r1, r1, r2
    adcs    r0, r0, r0
    .endr

    @ and then finally quit
    @ r0 = quotient, r1 = remainder
    bx      lr

@ r0: the number to get the reciprocal of
@ after it, r0 = (1 << 32)/value
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global ureciprocal32
    .type ureciprocal32 STT_FUNC
ureciprocal32:
    @ check if r0 is 0 or 1, because it will overflow or divide by 0
    cmp     r0, #1
    bls     __aeabi_idiv0

    @ move r0 = 0, r1 = 1 (so r0:r1 = 1 << 32) and r2 = -x
    rsb     r2, r0, #0
    mov     r1, #1
    movs    r0, r1, lsr #32 @ so r0 = 0, and clear the carry flag at the same time
    b       div32Iteration    @ the iteration is the same as for an unsigned division
