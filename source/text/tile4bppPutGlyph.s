@--------------------------------------------------------------------------------
@ tile4bppPutGlyph.s
@--------------------------------------------------------------------------------
@ Provides a way to write glyphs in VRAM for a correctly tilemapped region
@--------------------------------------------------------------------------------

@ for reference:
@ struct GlyphData final
@ {
@     const char* data; // 1bpp data
@     std::uint8_t height;
@     std::int8_t offsetX;
@     std::int8_t offsetY;
@     std::uint8_t advanceX:7;
@
@     // 0 for 16-bit bitfield, 1 for 32-bit bitfield
@     std::uint8_t attr:1;
@ };

@ void tile4bppPutGlyph(int x, int y, const GlyphData* gl, COLOR color, void* buffer, u32 pixelHeight);
@ x: r0, y: r1, gl: r2, color: r3, buffer: sp[0], pixelHeight: sp[4]
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global tile4bppPutGlyph
    .type tile4bppPutGlyph STT_FUNC
tile4bppPutGlyph:
    push    {r4-r9}                 @ Get necessary workspace for our function
    adr     r9, bitunpackLut        @ Preload the LUT used in bit unpacking down there
    ldmia   r2, {r4, r5}            @ Load the config words
    tst     r5, #255                @ Quick check to see if height is 0
    beq     .end

    mov     r2, r5, lsl #16         @ Last byte will contain gl->offsetX
    add     r0, r0, r2, asr #24     @ Sign extend gl->offsetX and add to x
    mov     r2, r5, lsl #8          @ Last byte will contain gl->offsetY
    add     r1, r1, r2, asr #24     @ Sign extend gl->offsetY and add to y

    @ Separate the X coordinate into its quotient and remainder by 8
    and     r6, r0, #7              @ now r6 = x%8
    mov     r0, r0, lsr #3          @ now r0 = x/8

    @ Calculate the first position in space
    ldr     r12, [sp, #24]           @ Load the buffer in r12
    ldr     r2, [sp, #28]           @ Load the pixel height in r2
    mla     r1, r0, r2, r1          @ y + pixelHeight*tileX
    add     r12, r12, r1, lsl #2    @ now buffer is &buffer[y + pixelHeight*tileX]

    @ Up to this point, we have
    @ r0: x/8 (not necessary anymore), r1: y + pixelHeight*tileX (not necessary anymore)
    @ r2: pixelHeight, r3: color, r4: buffer, r5: height, r6: x%8, r12: buffer

    @ so, we'll put a spare of the buffer in r0 and the character row in r1

    mov     r0, r12                 @ since r0 is not necessary anymore
    tst     r5, #1 << 31            @ check the attr flag
    and     r5, r5, #0xff           @ keep only the height
    bne     .wordsize               @ branch to the word routine

.hwordsize:
    @ Load the next 1bpp word
    ldrh    r1, [r4], #2            @ r1 = *(u16*)r4++
    mov     r1, r1, lsl r6          @ shift the result by the stride
.putglyphhword:
    and     r7, r1, #255            @ Take the next byte
    ldr     r7, [r9, r7, lsl #2]    @ LUT-unpack the bit
    mul     r8, r7, r3              @ Multiply by the color
    ldr     r7, [r12]               @ Load the older value
    orr     r7, r7, r8              @ Or both
    str     r7, [r12], r2, lsl #2   @ Advance by the pixel height (x4 for the byte height)

    @ Finally, shift the bytes in r1 to keep going
    movs    r1, r1, lsr #8
    bne     .putglyphhword

    add     r0, r0, #4              @ update the buffer
    mov     r12, r0
    subs    r5, r5, #1              @ subtract 1 from the height
    bne     .hwordsize              @ next iteration
    b       .end

.wordsize:
    @ Load the next 1bpp word
    push    {r10}
    ldr     r1, [r4], #4            @ r1 = *(u32*)r4++
    rsb     r6, r6, #32             @ r6 = 32 - r6
    mov     r10, r1, lsr r6         @ now r10 has the remaining part
    rsb     r6, r6, #32             
    mov     r1, r1, lsl r6          @ shift the result by the stride
.putglyphword:
    and     r7, r1, #255            @ Take the next byte
    ldr     r7, [r9, r7, lsl #2]    @ LUT-unpack the bit
    mul     r8, r7, r3              @ Multiply by the color
    ldr     r7, [r12]               @ Load the older value
    orr     r7, r7, r8              @ Or both
    str     r7, [r12], r2, lsl #2   @ Advance by the pixel height (x4 for the byte height)

    @ Finally, shift the bytes in r1 to keep going
    movs    r1, r1, lsr #8
    bne     .putglyphword

    @ Put the remaining bits into r1 and jump only if required
    movs    r1, r10
    bne     .putglyphword

    add     r0, r0, #4              @ update the buffer
    mov     r12, r0
    subs    r5, r5, #1              @ subtract 1 from the height
    bne     .wordsize               @ next iteration
    pop     {r10}

.end:
    pop     {r4-r9}
    bx      lr

bitunpackLut:
    .equ cnt, 0
    .rept 256
    .word (cnt&1) | ((cnt&2)<<3) | ((cnt&4)<<6) | ((cnt&8)<<9)\
        | ((cnt&16)<<12) | ((cnt&32)<<15) | ((cnt&64)<<18) | ((cnt&128)<<21)
    .equ cnt, cnt+1
    .endr
