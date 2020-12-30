@--------------------------------------------------------------------------------
@ collisionBitmask.s
@--------------------------------------------------------------------------------
@ Provides the collision detection routine with bitmasks
@--------------------------------------------------------------------------------

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global bitmaskBitmaskCollision
    .type bitmaskBitmaskCollision STT_FUNC
@ bool bitmaskBitmaskCollision(vec2<s16f7> pos1, const BitmaskData* b1, vec2<s16f7> pos2, const BitmaskData* b2);
@ pos1: r0, b1: r1, pos2: r2, b2: r3
bitmaskBitmaskCollision:
    push    {r4-r9}

    @ the long winded algorithm
    mov     r6, r0, lsl #16             @ this time, it's better if we do precise subtraction
    cmp     r6, r2, lsl #16             @ compare with the other word
    sub     r0, r0, r2                  @ first, get the delta position
    addlo   r0, r0, #1 << 16            @ compensate for the borrow if needed

    ldr     r4, [r1], #4                @ then, get the half sizes so we can compare
    ldr     r5, [r3], #4                @ the second half size

    add     r2, r4, r5                  @ we'll assume the sizes are small enough

    @ do a box check so we can skip the expensive phase
    movs    r6, r0, lsl #16             @ move the low word to r2
    rsbmi   r6, r6, #0                  @ and flip the sign if negative
    cmp     r6, r2, lsl #16             @ compare with the low half size
    bgt     .cfend                      @ quit if it's greater already
    movs    r6, r0, asr #16             @ sign-extend the high word
    rsbmi   r6, r6, #0                  @ remove the sign
    cmp     r6, r2, asr #16             @ compare with the high half size
    bgt     .cfend

    @ now transform center+hs in top-left + size
    @ tl1 = r1 - hs1, tl2 = r2 - hs2
    @ => tl1 - tl2 = (r1 - r2) - (hs1 - hs2)
    mov     r6, r4, lsl #16             @ subtract with borrow compensation
    cmp     r6, r5, lsl #16             @ compare hs1.x with hs2.x
    sub     r2, r4, r5                  @ hs1 - hs2
    addlo   r2, r2, #1 << 16            @ compensate the borrow
    mov     r6, r0, lsl #16
    cmp     r6, r2, lsl #16
    sub     r0, r0, r2                  @ now subtract it to the position
    addlo   r0, r0, #1 << 16            @ compensating for the carry
    mvn     r2, #1 << 16                @ now, prepare a mask to double the sizes
    and     r4, r2, r4, lsl #1          @ double the first half size
    and     r5, r2, r5, lsl #1          @ double the second half size

    @ the rest of the algorithm will assume that x1 >= x2
    @ so swap the parameters if that's not the case
    movs    r6, r0, lsl #16             @ this is x1 - x2
    bge     .noSwap                     @ don't swap if not needed
    eor     r0, r0, r6, lsr #16         @ clear the low hword of r0
    rsb     r6, r6, #0                  @ negate it
    rsb     r0, r0, #0                  @ negate the y part that is still in r0
    orr     r0, r6, lsr #16             @ and inject the x again in the register

    @ xor-swap the pointers and the sizes
    eor     r1, r1, r3
    eor     r3, r1, r3
    eor     r1, r1, r3

    eor     r4, r4, r5
    eor     r5, r4, r5
    eor     r4, r4, r5

.noSwap:
    @ get the number of clusters for each part
    mov     r7, r4, lsl #16             @ mask the 16 upper bits
    sub     r7, r7, #1 << 16            @ ceil(n/16) = 1 + floor((n-1/128)/16)
    mov     r7, r7, lsr #27
    add     r7, r7, #1
    mov     r9, r5, lsl #16
    sub     r9, r9, #1 << 16
    mov     r9, r9, lsr #27
    add     r9, r9, #1

    @ now x1 >= x2
    movs    r6, r6, lsr #27             @ get the number of clusters to adjust p2
    add     r3, r3, r6, lsl #1          @ add it to the second pointer
    sub     r0, r0, r6, lsl #11         @ and subtract it from the offset
    sub     r8, r9, r6                  @ and from the second size (storing it in a separate pointer)

    @ now 0 <= x1-x2 < 16 and p2 is correctly adjusted
    @ vertical adjustment now
    movs    r6, r0, asr #23             @ get the y difference
    mulpl   r2, r9, r6                  @ if y1 > y2, adjust p2
    addpl   r3, r3, r2, lsl #1
    subpl   r5, r5, r6, lsl #23
    mulmi   r2, r7, r6                  @ if y1 < y2, adjust p1
    submi   r1, r1, r2, lsl #1
    addmi   r4, r4, r6, lsl #23

    @ so far: r0 = x1 - x2 + garbage on the top word
    @         r1 = p1, r4 = size1 (adjusted)
    @         r3 = p2, r5 = size2 (adjusted)
    @         so r2 and r6 are free
    @         and 0 <= r0 < 16
    @ finally, get the minimum between the two sizes, first the x
    mov     r2, r7
    cmp     r2, r8
    movge   r2, r8
    cmp     r2, #0
    ble     .cfend

    @ then the y
    mov     r6, r4, lsr #23
    cmp     r6, r5, lsr #23
    movge   r6, r5, lsr #23
    cmp     r6, #0
    ble     .cfend

    @ now, r1 = p1, r3 = p2
    @ r2 is the number of words to consider on each cluster
    @ r6 is the number of clusters
    @ r7 and r8 are the number of clusters on each side
    @ r9 is the stride *without* adjustment
    @ r0 is the offset to give p1 on each part

    @ mask the upper 16 bits of r0 and get its integer part
    mov     r0, r0, lsl #16
    mov     r0, r0, lsr #23

    @ subtract the minimum from the clusters to get the difference
    sub     r7, r7, r2
    sub     r9, r9, r2

    @ now we can iterate through each cluster
.forEachHeight:
    subs    r12, r2, #1                 @ load one less iteration, the final iteration is special
    ldrh    r5, [r3], #2                @ load the first word of the second cluster
    beq     .endClusterLoop             @ go straight to the final iteration if needed

.clusterLoop:
    ldrh    r4, [r1], #2                @ load a word of the first cluster
    mov     r4, r4, lsl r0              @ offset it
    tst     r4, r5                      @ test for collision
    bne     .ctend                      @ finish if collision found
    ldrh    r5, [r3], #2                @ load the next word of the second cluster
    tst     r4, r5, lsl #16             @ test the offset word
    bne     .ctend                      @ and finish if found
    subs    r12, r12, #1                @ test for the next cluster
    bne     .clusterLoop

.endClusterLoop:
    ldrh    r4, [r1], #2                @ do the final iteration aside
    mov     r4, r4, lsl r0              @ offset it
    tst     r4, r5                      @ test for collision
    bne     .ctend                      @ finish if collision found
    cmp     r8, r2                      @ only do the last comparison if r8 is not the minimum
    ldrneh  r5, [r3]                    @ load the next word of the second cluster without updating the pointer
    tstne   r4, r5, lsl #16             @ test the offset word
    bne     .ctend                      @ and finish if found (there won't be a spurious return here)

    add     r1, r1, r7, lsl #1          @ proceed to the next cluster
    add     r3, r3, r9, lsl #1
    subs    r6, r6, #1
    bne     .forEachHeight

.cfend:                                 @ no collision
    mov     r0, #0
    pop     {r4-r9}
    bx      lr

.ctend:
    mov     r0, #1
    pop     {r4-r9}
    bx      lr


@================================================================================
    .extern CircleBitmasks                  @ array containing the pointers to the circle bitmasks
    .extern assertRadiusDoesNotExist

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global circleBitmaskCollision
    .type circleBitmaskCollision STT_FUNC
@ bool circleBitmaskCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, const BitmaskData* b2);
circleBitmaskCollision:
    mov     r1, r1, lsl #16                 @ mask the upper 16 bits
    movs    r1, r1, asr #23                 @ get the integer portion of the radius
    movle   r0, #0                          @ bail out if it's zero or negative
    bxle    lr

    push    {r5}
    ldr     r5, =CircleBitmasks             @ load the bitmask array into memory
    ldr     r5, [r5, r1, lsl #2]            @ load the bitmask corresponding to the entry
    cmp     r5, #0
    beq     assertRadiusDoesNotExist        @ bail out if it is zero

    @ And now branch to the bitmask routine
    mov     r1, r5
    pop     {r5}
    b       bitmaskBitmaskCollision

@================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global bitmaskCircleCollision
    .type bitmaskCircleCollision STT_FUNC
@ bool circleBitmaskCollision(vec2<s16f7> pos1, s16f7 r1, vec2<s16f7> pos2, const BitmaskData* b2);
bitmaskCircleCollision:
    mov     r3, r3, lsl #16                 @ mask the upper 16 bits
    movs    r3, r3, asr #23                 @ get the integer portion of the radius
    movle   r0, #0                          @ bail out if it's zero or negative
    bxle    lr

    push    {r5}
    ldr     r5, =CircleBitmasks             @ load the bitmask array into memory
    ldr     r5, [r5, r3, lsl #2]            @ load the bitmask corresponding to the entry
    cmp     r5, #0
    beq     assertRadiusDoesNotExist        @ bail out if it is zero

    @ And now branch to the bitmask routine
    mov     r3, r5
    pop     {r5}
    b       bitmaskBitmaskCollision
