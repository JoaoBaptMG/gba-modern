@--------------------------------------------------------------------------------
@ collision.cpp
@--------------------------------------------------------------------------------
@ Provide collision detection routines and utilities
@--------------------------------------------------------------------------------

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global circleCircleCollision
    .type circleCircleCollision STT_FUNC
@ bool circleCircleCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, s16f7 r2);
@ pos1: r0, r1: r1, pos2: r2, r3: r3
circleCircleCollision:
    mov     r3, r3, lsl #16             @ mask the last words of the radii
    add     r3, r3, r1, lsl #16         @ and sum them
    mov     r3, r3, lsr #16             @ put them back at the low word
    mul     r1, r3, r3                  @ now r1 is radius squared
    sub     r0, r0, r2                  @ subtract the vector
    @ NB: normally we should do the adjustment since it's not a full
    @ 32-bit word, but two 16-bit hwords fit into a single register
    @ but since the "discrepancy" is of 1/128th of a pixel, it's barely
    @ perceptible and we can win 3 cycles by ignoring it

    @ now r0 = pos1-pos2 and r1 = (r1+r2)**2
    mov     r2, r0, asr #16             @ pick the high word
    mul     r3, r2, r2                  @ square it
    mov     r2, r0, lsl #16             @ now sign extend the low word
    mov     r2, r2, asr #16             @ and square it
    mla     r3, r2, r2, r3              @ now r3 = |pos1-pos2|**2
    cmp     r3, r1                      @ compare it with r0
    movle   r0, #1                      @ and set it to the result
    movgt   r0, #0
    bx      lr

@================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global boxBoxCollision
    .type boxBoxCollision STT_FUNC
@ bool boxBoxCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, vec2<s16f7> s2);
@ pos1: r0, s1: r1, pos2: r2, s2: r3
boxBoxCollision:
    add     r1, r1, r3                  @ Add the size vectors neglecting the carry
    sub     r0, r0, r2                  @ subtract the position vectors neglecting the borrow
    movs    r2, r0, lsl #16             @ move the low word to r2
    rsbmi   r2, r2, #0                  @ and flip the sign if negative
    cmp     r2, r1, lsl #16             @ compare with the low half size
    bgt     .bbend                      @ quit if it's greater already
    movs    r2, r0, asr #16             @ sign-extend the high word
    rsbmi   r2, r2, #0                  @ remove the sign
    cmp     r2, r1, asr #16             @ compare with the high half size
    movle   r0, #1                      @ move the result
.bbend:
    movgt   r0, #0
    bx      lr

@================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global circleBoxCollision
    .type circleBoxCollision STT_FUNC
@ bool circleBoxCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, vec2<s16f7> s2);
@ pos1: r0, r1: r1, pos2: r2, s2: r3
circleBoxCollision:
    mov     r1, r1, lsl #16             @ mask the high bits of the radius
    orr     r1, r1, lsr #16             @ duplicate the radius on the lower hword
    add     r3, r3, r1                  @ add the size vectors (again neglecting carry)
    sub     r0, r0, r2                  @ subtract the positions
    movs    r2, r0, lsl #16             @ take the absolute value of the low word
    rsbmi   r2, r2, #0
    movs    r0, r0, asr #16             @ and the absolute value of the high word
    rsbmi   r0, r0, #0
    mov     r0, r0, lsl #16             @ join the words back together
    orr     r0, r2, lsr #16

    @ the outer box check first
    movs    r2, r0, lsl #16             @ move the low word to r2
    cmp     r2, r3, lsl #16             @ compare with the low half size
    bgt     .bbend2                     @ quit if it's greater already
    movs    r2, r0, asr #16             @ sign-extend the high word
    cmp     r2, r3, asr #16             @ compare with the high half size
    bgt     .bbend2

    @ now restore the original box size, and check for the original radii
    sub     r3, r3, r1
    movs    r2, r0, lsl #16             @ move the low word to r2
    cmp     r2, r3, lsl #16             @ compare with the low half size
    ble     .bbend2                     @ quit if it's lower
    movs    r2, r0, asr #16             @ sign-extend the high word
    cmp     r2, r3, asr #16             @ compare with the high half size
    ble     .bbend2

    @ finally, we're going to do a quick circle check if the point is within in the box of s2 and s2+r1
    mov     r1, r1, lsr #16             @ restore the radius again
    mul     r2, r1, r1                  @ put its square in r2
    sub     r0, r0, r3                  @ set r0 as the absolute value minus the box size
    mov     r1, r0, asr #16             @ pick up the high word
    mul     r3, r1, r1                  @ square it
    mov     r1, r0, lsl #16             @ pick up the low word
    mov     r1, r1, asr #16
    mla     r3, r1, r1, r3              @ and add to the squared word
    cmp     r3, r2                      @ and compare both              

.bbend2:
    movle   r0, #1                      @ move the result
    movgt   r0, #0
    bx      lr


    @================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global boxCircleCollision
    .type boxCircleCollision STT_FUNC
@ bool boxCircleCollision(vec2<s16f7> pos1, vec2<s16f7> s1, vec2<s16f7> pos2, s16f7 r2);
@ pos1: r0, s1: r1, pos2: r2, r2: r3
boxCircleCollision:
    @ swap the positions and sizes and jump to the upper routine
    eor     r0, r0, r2
    eor     r2, r0, r2
    eor     r0, r0, r2
    eor     r1, r1, r3
    eor     r3, r1, r3
    eor     r1, r1, r3 
    b       circleBoxCollision
