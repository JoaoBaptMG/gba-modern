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
    push    {r4}

    @ Now r3, r4 and r12 act as the holders for the data structure
.keepUpdating:
    @ Fetch the new projectile
    ldmia   r1, {r3-r4}         @ r3 = pos, r4 = vel
.keepUpdatingAfterRemoving:
    add     r3, r3, r4          @ Add disregarding the separation between the halfwords first
    mov     r12, r3, lsl #16    @ Store the lower halfword of the result
    cmp     r12, r4, lsl #16    @ Compare with the lower halfword of the velocity
    sublo   r3, r3, #1 << 16    @ and subtract the carry if it's higher

    adds    r12, r12, #8 << 23  @ add 8 to the x and check if it's still negative (essentially -8 <= x < 248)
    bmi     .remove

    mov     r12, r3, asr #16    @ move the position to the halfword
    subs    r12, r12, #80 << 7  @ subtract the half-height
    rsblt   r12, r12, #0        @ negate if it's negative to get the abs
    cmp     r12, #88 << 7       @ compare to the half-height + 8
    bge     .remove             @ and remove if it's greater (the projectile is offscreen

    str     r3, [r1]            @ store the position        
    add     r1, r1, #12         @ go to the next structure
    subs    r2, r2, #1          @ subtract 1 from the counter
    bne     .keepUpdating       @ and jump back if there are still projectiles
    b       .finish             @ jump to the end

.remove:
    @ fact: r1 + r2*12 will always be equal to the end of the array, since 12 is sizeof(Projectile)
    add     r12, r1, r2, lsl #3                 @ get the final pointer
    add     r12, r12, r2, lsl #2                @ (r2 << 3) + (r2 << 2) = r2 * 12
    ldmdb   r12, {r3-r4, r12}                   @ load the last projectile
    stmia   r1, {r3-r4, r12}                    @ store the original projectile data
    sub     r0, r0, #1                          @ reduce by one the number of projectiles
    subs    r2, r2, #1                          @ subtract 1 from the counter
    bne     .keepUpdatingAfterRemoving          @ and jump back if there are still projectiles

.finish:
    pop     {r4}                @ Pop back the registers to the stack
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

    push    {r4-r9}             @ Push the necessary registers

.resort:
    mov     r12, #1             @ use r12 as a sorted flag
    sub     r2, r0, #1          @ move the counter to r2
    ldrsh   r3, [r1], #12       @ load the first x position

.forwardLoop1:
    ldrsh   r4, [r1], #12       @ first half: r3 = prev pos, r4 = next pos
    cmp     r3, r4              @ if r3 < r4, then swap the memory
.cmpLabel1:                     @ set hooks for self-modifying code
    blt     .swapForward1       @ this instruction will be modified to change the sorting mode

.forwardLoop2:
    subs    r2, r2, #1          @ reduce the counter
    beq     .finishForwardLoop  @ and finish if zero

    ldrsh   r3, [r1], #12       @ second half: r4 = prev pos, r3 = next pos
    cmp     r4, r3              @ if r4 < r3, then swap the memory
.cmpLabel2:
    blt     .swapForward2

.forwardLoop3:
    subs    r2, r2, #1          @ reduce the counter
    bne     .forwardLoop1       @ and go back if there are still things to do

.finishForwardLoop:
    cmp     r12, #0             @ check if the array was already sorted
    bne     .finish2            @ go to the finish if so

    @ prepare for the backward loop
    sub     r2, r0, #1          @ move the counter to r2 again
    ldrsh   r3, [r1, #-12]!     @ load the last x position

.backwardLoop1:
    ldrsh   r4, [r1, #-12]!     @ first half: r3 = next pos, r4 = prev pos
    cmp     r4, r3              @ if r4 < r3, then swap the memory
.cmpLabel3:
    blt     .swapBackward1

.backwardLoop2:
    subs    r2, r2, #1          @ reduce the counter
    beq     .finishBackwardLoop @ and finish if zero

    ldrsh   r3, [r1, #-12]!     @ second half: r4 = next pos, r3 = prev pos
    cmp     r3, r4              @ if r3 < r4, then swap the memory
.cmpLabel4:
    blt     .swapBackward2

.backwardLoop3:
    subs    r2, r2, #1          @ reduce the counter
    bne     .backwardLoop1      @ and go back if there are still things to do

.finishBackwardLoop:
    cmp     r12, #0             @ check if the array was unsorted
    beq     .resort             @ resort if so

.finish2:
    pop     {r4-r9}             @ Pop the registers
    bx      lr

.swapForward1:
    mov     r4, r3
    ldmdb   r1!, {r3, r5-r6}    @ load the second projectile
    ldmdb   r1, {r7-r9}         @ load the first projectile
    stmdb   r1, {r3, r5-r6}     @ store the second into the first
    stmia   r1!, {r7-r9}        @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .forwardLoop2       @ and go back

.swapForward2:
    mov     r3, r4
    ldmdb   r1!, {r4-r6}        @ load the second projectile
    ldmdb   r1, {r7-r9}         @ load the first projectile
    stmdb   r1, {r4-r6}         @ store the second into the first
    stmia   r1!, {r7-r9}        @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .forwardLoop3       @ and go back

.swapBackward1:
    mov     r4, r3
    ldmia   r1!, {r3, r5-r6}    @ load the second projectile
    ldmia   r1, {r7-r9}         @ load the first projectile
    stmia   r1, {r3, r5-r6}     @ store the second into the first
    stmdb   r1!, {r7-r9}        @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .backwardLoop2      @ and go back

.swapBackward2:
    mov     r3, r4
    ldmia   r1!, {r4-r6}        @ load the second projectile
    ldmia   r1, {r7-r9}         @ load the first projectile
    stmia   r1, {r4-r6}         @ store the second into the first
    stmdb   r1!, {r7-r9}        @ store the first into the second
    mov     r12, #0             @ turn the sorted flag off
    b       .backwardLoop3      @ and go back

@===================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global setProjectileSortMode
    .type setProjectileSortMode STT_FUNC
setProjectileSortMode:          @ r0 is the mnemonic to put in, 0xCA for bgt and 0xBA for blt
    strb    r0, .cmpLabel1+3    @ set only the higher byte of the instruction
    strb    r0, .cmpLabel2+3
    strb    r0, .cmpLabel3+3
    strb    r0, .cmpLabel4+3
    bx      lr

@===================================================================================
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global pushProjectilesToOam
    .type pushProjectilesToOam STT_FUNC
pushProjectilesToOam:
    cmp     r0, #0                  @ check if there are projectiles around
    bxeq    lr                      @ and bail out if not

                                    @ r0 is the number of projectiles, r1 is the array,
    push    {r4-r10}                @ r2 is the first entry in the OAM, r3 is the projectile type array
    ldr     r10, [sp, #28]          @ sp+28 is the precomputed attr2add and the previous numObjects

    add     r3, #8                  @ adjust the projectile type so it's offset to the base
    mov     r9, #255                @ build the flag mask
    orr     r9, r9, #255 << 16  
    orr     r9, r9, #256 << 16

.sendProjectile:
    @ r4: projectile pos, r6: projectile type (and arg), r5-r7 are the attributes
    ldmia   r1!, {r4-r6}            @ load the projectile data
    mov     r6, r6, lsl #16         @ mask the last 16 bits
    orr     r6, r6, r6, lsr #2      @ now r6 = (20 * id) << 12
    add     r6, r3, r6, lsr #12     @ now r6 = &ptypeArray[id]
    ldm     r6, {r5-r7}             @ load attr0:attr1, attr2:prio, hsizex:hsizey

    @ subtract the position and account for carry
    mov     r8, r4, lsl #16         @ r8:H = pos.x
    cmp     r8, r7, lsl #16         @ compare pos.x and hsizex
    sub     r4, r4, r7
    addlo   r4, r4, #1 << 16        @ add back the borrow if pos.x < hsizex

    @ now move the position to the right place
    mov     r8, r4, lsr #23         @ the y should be in the low part
    orr     r8, r4, lsl #9          @ and the x in the right part
    and     r8, r8, r9              @ mask the values with the precomputed mask
    orr     r5, r5, r8              @ and put them in the attr0:attr1 thing
    add     r6, r6, r10             @ add the attr2add plus the object number
    stmia   r2!, {r5-r6}            @ and push it to the OAM
    
    add     r10, # 1 << 16          @ increase the current object number by one
    subs    r0, #1                  @ reduce by one the number of iterations
    bne     .sendProjectile         @ and go back if there are still more

    pop     {r4-r10}
    bx      lr
