@--------------------------------------------------------------------------------
@ tile4bppPutGlyph.s
@--------------------------------------------------------------------------------
@ Provides a way to write glyphs in VRAM for a correctly tilemapped region
@--------------------------------------------------------------------------------

@ for reference:
@ struct GlyphData final
@ {
@     std::uint8_t height;
@     std::int8_t offsetX;
@     std::int8_t offsetY;
@     std::uint8_t advanceX:7;
@
@     // 0 for 16-bit bitfield, 1 for 32-bit bitfield
@     std::uint8_t attr:1;
@     const char* data; // 1bpp data
@ };

@ void tile4bppPutGlyph(int x, int y, const GlyphData* gl, COLOR color, void* buffer, u32 pixelHeight);
@ x: r0, y: r1, gl: r2, color: r3, buffer: sp[0], pixelHeight: sp[4]
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global tile4bppPutGlyph
    .type tile4bppPutGlyph STT_FUNC
tile4bppPutGlyph:
    push    {r4-r10}                @ Get necessary workspace for our function
    ldr     r10, =0x01010101        @ Preload the auxiliary mask used in the loop down there
    ldmia   r2, {r4, r5}            @ Load the config words
    tst     r4, #255                @ Quick check to see if height is 0
    beq     .end

    mov     r2, r4, lsl #16         @ Last byte will contain gl->offsetX
    add     r0, r0, r2, asr #24     @ Sign extend gl->offsetX and add to x
    mov     r2, r4, lsl #8          @ Last byte will contain gl->offsetY
    add     r1, r1, r2, asr #24     @ Sign extend gl->offsetY and add to y

    @ Separate the X coordinate into its quotient and remainder by 8
    and     r6, r0, #7              @ now r6 = x%8
    mov     r0, r0, lsr #3          @ now r0 = x/8

    @ Calculate the first position in space
    ldr     r12, [sp, #28]          @ Load the buffer in r12
    ldr     r2, [sp, #32]           @ Load the pixel height in r2
    mla     r1, r0, r2, r1          @ y + pixelHeight*tileX
    add     r12, r12, r1, lsl #2    @ now buffer is &buffer[y + pixelHeight*tileX]

    @ Up to this point, we have
    @ r0: x/8 (not necessary anymore), r1: y + pixelHeight*tileX (not necessary anymore)
    @ r2: pixelHeight, r3: color, r4: height, r5: buffer, r6: x%8, r12: buffer

    @ so, we'll put a spare of the buffer in r0 and the character row in r1
    mov     r0, r12                 @ since r0 is not necessary anymore
    tst     r4, #1 << 31            @ check the attr flag
    and     r4, r4, #0xff           @ keep only the height
    bne     .wordsize               @ branch to the word routine

.hwordsize:
    @ Load the next 1bpp word
    ldrh    r1, [r5], #2            @ r1 = *(u16*)r5++
    mov     r1, r1, lsl r6          @ shift the result by the stride
.putglyphhword:
    and     r7, r1, #255            @ Take the next byte
    orr     r7, r7, r7, lsl #12     @ Shift the higher nibbles into their place...
    orr     r7, r7, r7, lsl #6      @ ...in order to be able to apply the mask
    and     r8, r7, r10, lsl #1     @ Pick up the even bits
    and     r7, r7, r10             @ Pick up the odd bits
    orr     r7, r7, r8, lsl #3      @ Combine them into a single 4-bit unpacked value 
    mul     r8, r7, r3              @ Multiply by the color
    rsb     r9, r7, r7, lsl #4      @ Multiply by F so it can be used as a mask
    ldr     r7, [r12]               @ Load the older value
    bic     r7, r7, r9              @ Clear the area that should be filled
    orr     r7, r7, r8              @ Fill the area with the pixel color
    str     r7, [r12], r2, lsl #2   @ Advance by the pixel height (x4 for the byte height)

    @ Finally, shift the bytes in r1 to keep going
    movs    r1, r1, lsr #8
    bne     .putglyphhword

    add     r0, r0, #4              @ update the buffer
    mov     r12, r0
    subs    r4, r4, #1              @ subtract 1 from the height
    bne     .hwordsize              @ next iteration
    b       .end

.wordsize:
    @ Load the next 1bpp word
    push    {r11}
    ldr     r1, [r5], #4            @ r1 = *(u32*)r5++
    rsb     r6, r6, #32             @ r6 = 32 - r6
    mov     r11, r1, lsr r6         @ now r11 has the remaining part
    rsb     r6, r6, #32             
    mov     r1, r1, lsl r6          @ shift the result by the stride
.putglyphword:
    and     r7, r1, #255            @ Take the next byte
    orr     r7, r7, r7, lsl #12     @ Shift the higher nibbles into their place...
    orr     r7, r7, r7, lsl #6      @ ...in order to be able to apply the mask
    and     r8, r7, r10, lsl #1     @ Pick up the even bits
    and     r7, r7, r10             @ Pick up the odd bits
    orr     r7, r7, r8, lsl #3      @ Combine them into a single 4-bit unpacked value 
    mul     r8, r7, r3              @ Multiply by the color
    rsb     r9, r7, r7, lsl #4      @ Multiply by F so it can be used as a mask
    ldr     r7, [r12]               @ Load the older value
    bic     r7, r7, r9              @ Clear the area that should be filled
    orr     r7, r7, r8              @ Fill the area with the pixel color
    str     r7, [r12], r2, lsl #2   @ Advance by the pixel height (x4 for the byte height)

    @ Finally, shift the bytes in r1 to keep going
    movs    r1, r1, lsr #8
    bne     .putglyphword

    @ Put the remaining bits into r1 and jump only if required
    movs    r1, r11
    bne     .putglyphword

    add     r0, r0, #4              @ update the buffer
    mov     r12, r0
    subs    r4, r4, #1              @ subtract 1 from the height
    bne     .wordsize               @ next iteration
    pop     {r11}

.end:
    pop     {r4-r10}
    bx      lr

