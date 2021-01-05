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
