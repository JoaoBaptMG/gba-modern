@--------------------------------------------------------------------------------
@ sqrt64.s
@--------------------------------------------------------------------------------
@ Provides an implementation of a square root algorithm for 64-bits
@--------------------------------------------------------------------------------

@ Source code is an adaption from the code seen in http://www.pertinentdetail.org/sqrt
@ u64 isqrt32(u64 x) // so x is in r0:r1
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global isqrt64
    .type isqrt64 STT_FUNC
isqrt64:
    cmp     r1, #0      @ if the high bits are zero, just branch to the 32-bit code
    beq     isqrt32

    @ now we have r1:r2 is our 64-bit value, r3 is our test
    @ and r0 is our root
    mov     r2, r1
    mov     r0, #0

    @ short-circuit to special cases where some cycles can be shaved off
    movs    r3, r2, lsr #8     @ if the last three bytes are 0
    beq     .sqrt40            @ move to the 40-bit chunk
    movs    r3, r2, lsr #16    @ if the last two bytes are 0
    beq     .sqrt48            @ move to the 48-bit chunk
    movs    r3, r2, lsr #24    @ if the last byte is 0
    beq     .sqrt56            @ move to the 56-bit chunk

    @ now, the looooong unrolling of the loop
    cmp     r1, #1 << 30
    cmpeq   r2, #0
    subhs   r1, #1 << 30
    addhs   r0, r0, #1 << 31

    @ i = 30
    add     r3, r0, #1 << 29
    cmp     r1, r3, lsr #1
    cmpeq   r2, r3, lsl #31
    blo     .sqrt60
    subs    r2, r3, lsl #31
    sbc     r1, r3, lsr #1
    add     r0, r0, #1 << 30
.sqrt60:

    @ i = 29
    add     r3, r0, #1 << 28
    cmp     r1, r3, lsr #2
    cmpeq   r2, r3, lsl #30
    blo     .sqrt58
    subs    r2, r3, lsl #30
    sbc     r1, r3, lsr #2
    add     r0, r0, #1 << 29
.sqrt58:

    @ i = 28
    add     r3, r0, #1 << 27
    cmp     r1, r3, lsr #3
    cmpeq   r2, r3, lsl #29
    blo     .sqrt56
    subs    r2, r3, lsl #29
    sbc     r1, r3, lsr #3
    add     r0, r0, #1 << 28
.sqrt56:

    @ i = 27
    add     r3, r0, #1 << 26
    cmp     r1, r3, lsr #4
    cmpeq   r2, r3, lsl #28
    blo     .sqrt54
    subs    r2, r3, lsl #28
    sbc     r1, r3, lsr #4
    add     r0, r0, #1 << 27
.sqrt54:

    @ i = 26
    add     r3, r0, #1 << 25
    cmp     r1, r3, lsr #5
    cmpeq   r2, r3, lsl #27
    blo     .sqrt52
    subs    r2, r3, lsl #27
    sbc     r1, r3, lsr #5
    add     r0, r0, #1 << 26
.sqrt52:

    @ i = 25
    add     r3, r0, #1 << 24
    cmp     r1, r3, lsr #6
    cmpeq   r2, r3, lsl #26
    blo     .sqrt50
    subs    r2, r3, lsl #26
    sbc     r1, r3, lsr #6
    add     r0, r0, #1 << 25
.sqrt50:

    @ i = 24
    add     r3, r0, #1 << 23
    cmp     r1, r3, lsr #7
    cmpeq   r2, r3, lsl #25
    blo     .sqrt48
    subs    r2, r3, lsl #25
    sbc     r1, r3, lsr #7
    add     r0, r0, #1 << 24
.sqrt48:

    @ i = 23
    add     r3, r0, #1 << 22
    cmp     r1, r3, lsr #8
    cmpeq   r2, r3, lsl #24
    blo     .sqrt46
    subs    r2, r3, lsl #24
    sbc     r1, r3, lsr #8
    add     r0, r0, #1 << 23
.sqrt46:

    @ i = 22
    add     r3, r0, #1 << 21
    cmp     r1, r3, lsr #9
    cmpeq   r2, r3, lsl #23
    blo     .sqrt44
    subs    r2, r3, lsl #23
    sbc     r1, r3, lsr #9
    add     r0, r0, #1 << 22
.sqrt44:

    @ i = 21
    add     r3, r0, #1 << 20
    cmp     r1, r3, lsr #10
    cmpeq   r2, r3, lsl #22
    blo     .sqrt42
    subs    r2, r3, lsl #22
    sbc     r1, r3, lsr #10
    add     r0, r0, #1 << 21
.sqrt42:

    @ i = 20
    add     r3, r0, #1 << 19
    cmp     r1, r3, lsr #11
    cmpeq   r2, r3, lsl #21
    blo     .sqrt40
    subs    r2, r3, lsl #21
    sbc     r1, r3, lsr #11
    add     r0, r0, #1 << 20
.sqrt40:

    @ i = 19
    add     r3, r0, #1 << 18
    cmp     r1, r3, lsr #12
    cmpeq   r2, r3, lsl #20
    blo     .sqrt38
    subs    r2, r3, lsl #20
    sbc     r1, r3, lsr #12
    add     r0, r0, #1 << 19
.sqrt38:

    @ i = 18
    add     r3, r0, #1 << 17
    cmp     r1, r3, lsr #13
    cmpeq   r2, r3, lsl #19
    blo     .sqrt36
    subs    r2, r3, lsl #19
    sbc     r1, r3, lsr #13
    add     r0, r0, #1 << 18
.sqrt36:

    @ i = 17
    add     r3, r0, #1 << 16
    cmp     r1, r3, lsr #14
    cmpeq   r2, r3, lsl #18
    blo     .sqrt34
    subs    r2, r3, lsl #18
    sbc     r1, r3, lsr #14
    add     r0, r0, #1 << 17
.sqrt34:

    @ i = 16
    add     r3, r0, #1 << 15
    cmp     r1, r3, lsr #15
    cmpeq   r2, r3, lsl #17
    blo     .sqrt32
    subs    r2, r3, lsl #17
    sbc     r1, r3, lsr #15
    add     r0, r0, #1 << 16
.sqrt32:

    @ i = 15
    add     r3, r0, #1 << 14
    cmp     r1, r3, lsr #16
    cmpeq   r2, r3, lsl #16
    blo     .sqrt30
    subs    r2, r3, lsl #16
    sbc     r1, r3, lsr #16
    add     r0, r0, #1 << 15
.sqrt30:

    @ i = 14
    add     r3, r0, #1 << 13
    cmp     r1, r3, lsr #17
    cmpeq   r2, r3, lsl #15
    blo     .sqrt28
    subs    r2, r3, lsl #15
    sbc     r1, r3, lsr #17
    add     r0, r0, #1 << 14
.sqrt28:

    @ i = 13
    add     r3, r0, #1 << 12
    cmp     r1, r3, lsr #18
    cmpeq   r2, r3, lsl #14
    blo     .sqrt26
    subs    r2, r3, lsl #14
    sbc     r1, r3, lsr #18
    add     r0, r0, #1 << 13
.sqrt26:

    @ i = 12
    add     r3, r0, #1 << 11
    cmp     r1, r3, lsr #19
    cmpeq   r2, r3, lsl #13
    blo     .sqrt24
    subs    r2, r3, lsl #13
    sbc     r1, r3, lsr #19
    add     r0, r0, #1 << 12
.sqrt24:

    @ i = 11
    add     r3, r0, #1 << 10
    cmp     r1, r3, lsr #20
    cmpeq   r2, r3, lsl #12
    blo     .sqrt22
    subs    r2, r3, lsl #12
    sbc     r1, r3, lsr #20
    add     r0, r0, #1 << 11
.sqrt22:

    @ i = 10
    add     r3, r0, #1 << 9
    cmp     r1, r3, lsr #21
    cmpeq   r2, r3, lsl #11
    blo     .sqrt20
    subs    r2, r3, lsl #11
    sbc     r1, r3, lsr #21
    add     r0, r0, #1 << 10
.sqrt20:

    @ i = 9
    add     r3, r0, #1 << 8
    cmp     r1, r3, lsr #22
    cmpeq   r2, r3, lsl #10
    blo     .sqrt18
    subs    r2, r3, lsl #10
    sbc     r1, r3, lsr #22
    add     r0, r0, #1 << 9
.sqrt18:

    @ i = 8
    add     r3, r0, #1 << 7
    cmp     r1, r3, lsr #23
    cmpeq   r2, r3, lsl #9
    blo     .sqrt16
    subs    r2, r3, lsl #9
    sbc     r1, r3, lsr #23
    add     r0, r0, #1 << 8
.sqrt16:

    @ i = 7
    add     r3, r0, #1 << 6
    cmp     r1, r3, lsr #24
    cmpeq   r2, r3, lsl #8
    blo     .sqrt14
    subs    r2, r3, lsl #8
    sbc     r1, r3, lsr #24
    add     r0, r0, #1 << 7
.sqrt14:

    @ i = 6
    add     r3, r0, #1 << 5
    cmp     r1, r3, lsr #25
    cmpeq   r2, r3, lsl #7
    blo     .sqrt12
    subs    r2, r3, lsl #7
    sbc     r1, r3, lsr #25
    add     r0, r0, #1 << 6
.sqrt12:

    @ i = 5
    add     r3, r0, #1 << 4
    cmp     r1, r3, lsr #26
    cmpeq   r2, r3, lsl #6
    blo     .sqrt10
    subs    r2, r3, lsl #6
    sbc     r1, r3, lsr #26
    add     r0, r0, #1 << 5
.sqrt10:

    @ i = 4
    add     r3, r0, #1 << 3
    cmp     r1, r3, lsr #27
    cmpeq   r2, r3, lsl #5
    blo     .sqrt8
    subs    r2, r3, lsl #5
    sbc     r1, r3, lsr #27
    add     r0, r0, #1 << 4
.sqrt8:

    @ i = 3
    add     r3, r0, #1 << 2
    cmp     r1, r3, lsr #28
    cmpeq   r2, r3, lsl #4
    blo     .sqrt6
    subs    r2, r3, lsl #4
    sbc     r1, r3, lsr #28
    add     r0, r0, #1 << 3
.sqrt6:

    @ i = 2
    add     r3, r0, #1 << 1
    cmp     r1, r3, lsr #29
    cmpeq   r2, r3, lsl #3
    blo     .sqrt4
    subs    r2, r3, lsl #3
    sbc     r1, r3, lsr #29
    add     r0, r0, #1 << 2
.sqrt4:

    @ i = 1
    add     r3, r0, #1 << 0
    cmp     r1, r3, lsr #30
    cmpeq   r2, r3, lsl #2
    blo     .sqrt2
    subs    r2, r3, lsl #2
    sbc     r1, r3, lsr #30
    add     r0, r0, #1 << 1
.sqrt2:

    @ i = 0 finally
    cmp     r1, r0, lsr #31
    moveq   r3, #1
    addeq   r3, r0, lsl #1
    cmpeq   r2, r3
    addhs   r0, r0, #1
    
    @ finally, go away
    mov     r1, #0
    bx lr
