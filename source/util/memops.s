@--------------------------------------------------------------------------------
@ memops.s
@--------------------------------------------------------------------------------
@ Provides an optimized version of memcpy, memset and memmove in assembly
@--------------------------------------------------------------------------------

@ void* memcpy(void *dst, const void *src, size_t cnt);
@ based on the memcpy16 function at https://github.com/devkitPro/libtonc/blob/master/asm/tonc_memcpy.s
@ dst: r0, src: r1, cnt: r2
    .text
    .align  2
    .thumb_func
    .global memcpy
    .type   memcpy STT_FUNC
memcpy:
    push    {r0, lr}

    cmp     r2, #16             @ If cnt is not high enough...
    bls     .copyLastBytes      @ ...do a standard halfword copy

    @ Compute dst^src, that will be used in the following routines
    mov     r3, r0
    eor     r3, r1
    lsl     r3, r3, #30         @ Preserve only the last 2 bits of dst^src
    bne     .copyLastBytes2     @ If they aren't zero, dst and src cannot be word aligned

    @ Load src's "remainder" bytes in r3 to do a strategy
    lsr     r3, r0, #1          @ Load bit 0 into carry
    bcc     .tryAlignHalfword   @ Copy a byte if necessary

    @ Copy a single byte
    ldrb    r3, [r1]
    strb    r3, [r0]
    add     r0, r0, #1
    add     r1, r0, #1
    sub     r2, r2, #1

.tryAlignHalfword:
    lsr     r3, r0, #2          @ Load bit 0 into carry
    bcc     .copyByWords        @ Copy a halfword if necessary

    @ Copy a single halfword
    ldrh    r3, [r1]
    strh    r3, [r0]
    add     r0, r0, #2
    add     r1, r0, #2
    sub     r2, r2, #2

.copyByWords:
    @ Prepare the memcpy32 routine to execute
    push    {r4}
    lsl     r4, r2, #30         @ Save the last two bits of cnt
    lsr     r2, r2, #2          @ Shift the cnt by 2 to make the word count
    ldr     r3, =memcpy32       @ Load the address of memcpy32 in r3
    bl      r3trampoline        @ ...and bx to it

    @ After memcpy32, r0 and r1 point to the values after the copy
    lsr     r2, r4, #30         @ Restore the last two bits
    pop     {r4}
    beq     .finishedCopy       @ If no bytes are remaining, bail out
    b       .copyByHalfwords    @ Go directly into the halfword copy

.copyLastBytes:
    mov     r3, r0
    eor     r3, r1
    lsl     r3, r3, #30         @ Preserve only the last 2 bits of dst^src
.copyLastBytes2:
    lsl     r3, r3, #2          @ Move bit 0 into carry
    bcs     .copyByBytes        @ They cannot be halfword aligned

    @ See if it is needed to hword-align the addresses
    lsr     r3, r0, #1          @ Load dst's bit 0 into carry
    bcc     .copyByHalfwords    @ If needed, align the bytes again

    @ Copy a single byte
    ldrb    r3, [r1]
    strb    r3, [r0]
    add     r0, r0, #1
    add     r1, r0, #1
    sub     r2, r2, #1

.copyByHalfwords:
    mov     r12, r2             @ Save cnt into r12
    lsr     r2, r2, #1
    sub     r2, r2, #1          @ Subtract to get the index
    bcc     .hwloopend          @ Bail out if there are no words to copy
    lsl     r2, r2, #1          @ Mask out r2's last bit

.hwloop:
    ldrh    r3, [r1, r2]
    strh    r3, [r0, r2]
    sub     r2, r2, #2
    bcs     .hwloop
.hwloopend:
    mov     r2, r12             @ Restore cnt
    mov     r3, #1              @ And cnt with 1
    and     r2, r3

.copyByBytes:
    sub     r2, r2, #1          @ Subtract to get the last index
    bcc     .finishedCopy       @ Bail out if there are no bytes to copy

.byteloop:
    ldrb    r3, [r1, r2]
    strb    r3, [r0, r2]
    sub     r2, r2, #1
    bcs     .byteloop

.finishedCopy:
    @ Pop lr into r3 to be able to bx from it
    pop     {r0, r3}            @ Pop the original dst (which needs to be returned)
r3trampoline:
    bx      r3

@ void* memset(void *dst, int val, size_t cnt);
@ based on the memcpy16 function at https://github.com/devkitPro/libtonc/blob/master/asm/tonc_memset.s
@ dst: r0, val: r1, cnt: r2
    .text
    .align  2
    .thumb_func
    .global memset
    .type   memset STT_FUNC
memset:
    push    {lr}

    @ Clamp r1 to the byte range
    mov     r3, #255
    and     r1, r3

    cmp     r2, #12             @ If cnt is not high enough...
    bls     .setLastBytes       @ ...do a standard halfword set

    @ If dst is not word aligned, copy a word and a byte if necessary
    lsr     r3, r0, #1          @ Load bit 0 into carry
    bcc     .tryAlignHalfwordS  @ Copy a byte if necessary

    strb    r1, [r0]
    add     r0, r0, #1
    sub     r2, r2, #1

.tryAlignHalfwordS:
    @ Extend r1 to the halfword
    lsl     r3, r1, #8
    orr     r1, r3

    lsr     r3, r0, #2          @ Load bit 1 into carry
    bcc     .setByWords         @ Copy a halfword if necessary

    strh    r1, [r0]
    add     r0, r0, #2
    sub     r2, r2, #2

.setByWords:
    @ Extend r1 to the word
    lsl     r3, r1, #16
    orr     r1, r3

    @ Prepare the memset32 routine to execute
    push    {r4}
    lsl     r4, r2, #30         @ Save the last two bits of cnt
    lsr     r2, r2, #2          @ Shift the cnt by 2 to make the word count
    ldr     r3, =memset32       @ Load the address of memset32 in r3
    bl      r3trampoline        @ ...and bx to it

    @ After memcpy32, r0 and r1 point to the values after the copy
    lsr     r2, r4, #30         @ Restore the last two bits
    pop     {r4}

.setLastBytes:
    sub     r2, r2, #1          @ Subtract to get the last index
    bcc     .finishedCopyS      @ Bail out if there are no bytes to copy

.sbyteloop:
    strb    r1, [r0, r2]
    sub     r2, r2, #1
    bcs     .sbyteloop

.finishedCopyS:
    @ Pop lr into r3 to be able to bx from it
    pop     {r3}
    bx      r3
