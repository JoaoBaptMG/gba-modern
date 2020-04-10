@--------------------------------------------------------------------------------
@ Projectile.s
@--------------------------------------------------------------------------------
@ Provides routines for manipulating projectiles
@--------------------------------------------------------------------------------

    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global updateProjectiles
    .type updateProjectiles STT_FUNC
updateProjectiles:              @ r0 is the projectile number, r1 is the pointer
    cmp     r0, #0              @ if there are no projectiles
    bxeq    lr                  @ Go away

    mov     r2, r0              @ Move the number of projectiles to r2, it will act as a counter
    push    {r4-r5}

    @ Now r3, r4, r5 and r12 act as the holders for the data structure
.keepUpdating:
    @ Fetch the new projectile
    ldmia   r1, {r3-r4}         @ r3 = pos, r4 = vel
.keepUpdatingAfterRemoving:
    add     r3, r3, r4          @ Add disregarding the separation between the halfwords first
    mov     r5, r3, lsl #16     @ Store the lower halfword of the result
    cmp     r5, r4, lsl #16     @ Compare with the lower halfword of the velocity
    sublo   r3, r3, #1 << 16    @ and subtract the carry if it's higher

    adds    r5, r5, #8 << 23    @ add 8 to the x and check if it's still negative (essentially -8 <= x < 248)
    bmi     .remove

    mov     r5, r3, asr #16     @ move the position to the halfword
    subs    r5, r5, #80 << 7    @ subtract the half-height
    rsblt   r5, r5, #0          @ negate if it's negative to get the abs
    cmp     r5, #88 << 7        @ compare to the half-height + 8
    bge     .remove             @ and remove if it's greater (the projectile is offscreen

    str     r3, [r1]            @ store the position        
    add     r1, r1, #16         @ go to the next structure
    subs    r2, r2, #1          @ subtract 1 from the counter
    bne     .keepUpdating       @ and jump back if there are still projectiles
    b       .finish             @ jump to the end

.remove:
    @ fact: r1 + r2*16 will always be equal to the end of the array, since 16 is sizeof(Projectile)
    add     r5, r1, r2, lsl #4                  @ get the final pointer
    ldmdb   r5, {r3-r5, r12}                    @ load the last projectile
    stmia   r1, {r3-r5, r12}                    @ store the original projectile data
    sub     r0, r0, #1                          @ reduce by one the number of projectiles
    subs    r2, r2, #1                          @ subtract 1 from the counter
    bne     .keepUpdatingAfterRemoving          @ and jump back if there are still projectiles

.finish:
    pop     {r4-r5}             @ Pop back the registers to the stack
    bx      lr

@===================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global sortProjectiles
    .type sortProjectiles STT_FUNC
sortProjectiles:                @ r0 is the projectile number, r1 is the pointer
    cmp     r0, #1              @ if there's only one projectile (or zero), bail out
    bxls    lr

    push    {r4-r11}            @ Push the necessary registers

.resort:
    mov     r12, #1             @ use r12 as a sorted flag
    sub     r2, r0, #1          @ move the counter to r2
    ldrsh   r3, [r1], #16       @ load the first x position

.forwardLoop1:
    ldrsh   r4, [r1], #16       @ first half: r3 = prev pos, r4 = next pos
    cmp     r3, r4              @ if r3 < r4, then swap the memory
    blt     .swapForward1

.forwardLoop2:
    subs    r2, r2, #1          @ reduce the counter
    beq     .finishForwardLoop  @ and finish if zero

    ldrsh   r3, [r1], #16       @ second half: r4 = prev pos, r3 = next pos
    cmp     r4, r3              @ if r4 < r3, then swap the memory
    blt     .swapForward2

.forwardLoop3:
    subs    r2, r2, #1          @ reduce the counter
    bne     .forwardLoop1       @ and go back if there are still things to do

.finishForwardLoop:
    cmp     r12, #0             @ check if the array was already sorted
    bne     .finish2            @ go to the finish if so

    @ prepare for the backward loop
    sub     r2, r0, #1          @ move the counter to r2 again
    ldrsh   r3, [r1, #-16]!     @ load the last x position

.backwardLoop1:
    ldrsh   r4, [r1, #-16]!     @ first half: r3 = next pos, r4 = prev pos
    cmp     r3, r4              @ if r3 > r4, then swap the memory
    bgt     .swapBackward1

.backwardLoop2:
    subs    r2, r2, #1          @ reduce the counter
    beq     .finishBackwardLoop @ and finish if zero

    ldrsh   r3, [r1, #-16]!     @ second half: r4 = next pos, r3 = prev pos
    cmp     r4, r3              @ if r4 > r3, then swap the memory
    bgt     .swapBackward2

.backwardLoop3:
    subs    r2, r2, #1          @ reduce the counter
    bne     .backwardLoop1      @ and go back if there are still things to do

.finishBackwardLoop:
    cmp     r12, #0             @ check if the array was unsorted
    beq     .resort             @ resort if so

.finish2:
    pop     {r4-r11}            @ Pop the registers
    bx      lr

.swapForward1:
    mov     r4, r3
    ldmdb   r1!, {r3, r5-r7}    @ load the second projectile
    ldmdb   r1, {r8-r11}        @ load the first projectile
    stmdb   r1, {r3, r5-r7}     @ store the second into the first
    stmia   r1!, {r8-r11}       @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .forwardLoop2       @ and go back

.swapForward2:
    mov     r3, r4
    ldmdb   r1!, {r4-r7}        @ load the second projectile
    ldmdb   r1, {r8-r11}        @ load the first projectile
    stmdb   r1, {r4-r7}         @ store the second into the first
    stmia   r1!, {r8-r11}       @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .forwardLoop3       @ and go back

.swapBackward1:
    mov     r4, r3
    ldmia   r1!, {r3, r5-r7}    @ load the second projectile
    ldmia   r1, {r8-r11}        @ load the first projectile
    stmia   r1, {r3, r5-r7}     @ store the second into the first
    stmdb   r1!, {r8-r11}       @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .backwardLoop2      @ and go back

.swapBackward2:
    mov     r3, r4
    ldmia   r1!, {r4-r7}        @ load the second projectile
    ldmia   r1, {r8-r11}        @ load the first projectile
    stmia   r1, {r4-r7}         @ store the second into the first
    stmdb   r1!, {r8-r11}       @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .backwardLoop3      @ and go back

