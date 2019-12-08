@--------------------------------------------------------------------------------
@ mode3PutGlyph.s
@--------------------------------------------------------------------------------
@ Provides a way to write glyphs in text for a mode-3 setting
@--------------------------------------------------------------------------------

@ for reference:
@struct GlyphData final
@{
@    const char* data; // 1bpp data
@    std::uint8_t height;
@    std::int8_t offsetX;
@    std::int8_t offsetY;
@    std::uint8_t advanceX:7;
@
@    // 0 for 16-bit bitfield, 1 for 32-bit bitfield
@    std::uint8_t attr:1;
@ };

@ void mode3PutGlyph(int x, int y, const GlyphData* gl, COLOR color, void* buffer);
@ x: r0, y: r1, gl: r2, color: r3, buffer: sp[0]
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global mode3PutGlyph
    .type mode3PutGlyph STT_FUNC
mode3PutGlyph:
    push    {r4, r5}                @ Get necessary workspace for our function
    ldmia   r2, {r4, r5}            @ Load the config words
    tst     r5, #255                @ Quick check to see if height is 0
    beq     .end

    mov     r2, r5, lsl #16         @ Last byte will contain gl->offsetX
    add     r0, r0, r2, asr #24     @ Sign extend gl->offsetX and add to x
    mov     r2, r5, lsl #8          @ Last byte will contain gl->offsetX
    add     r1, r1, r2, asr #24     @ Sign extend gl->offsetY and add to y

    @ Calculate the first position in space
    ldr     r12, [sp, #8]           @ Load the buffer in r12
    mov     r2, #240                @ Multiplier
    mla     r0, r1, r2, r0          @ 240*y + x
    add     r12, r12, r0, lsl #1    @ now buffer is &buffer[240*y + x]

    mov     r1, r12                 @ since r1 is not necessary anymore
    tst     r5, #1 << 31            @ check the attr flag
    and     r5, r5, #0xff           @ keep only the height
    bne     .wordsize               @ branch to the word routine

.hwordsize:
    @ Load the next 1bpp word
    ldrh    r2, [r4], #2            @ r2 = *(u16*)r4++
.putglyphhword:
    tst     r2, #1                  @ check if r2&1 is a thing
    strneh  r3, [r12]               @ put the color into the buffer
    add     r12, r12, #2            @ approach in the buffer
    movs    r2, r2, lsr #1          @ shift to the next pixel
    bne     .putglyphhword          @ keep putting glyphs

    add     r1, r1, #480            @ update the buffer
    mov     r12, r1
    subs    r5, r5, #1              @ subtract 1 from the height
    bne     .hwordsize              @ next iteration
    b       .end

.wordsize:
    @ Load the next 1bpp word
    ldr     r2, [r4], #2            @ r2 = *(u32*)r4++
.putglyphword:
    tst     r2, #1                  @ check if r2&1 is a thing
    strneh  r3, [r12]               @ put the color into the buffer
    add     r12, r12, #2            @ approach in the buffer
    movs    r2, r2, lsr #1          @ shift to the next pixel
    bne     .putglyphword           @ keep putting glyphs

    add     r1, r1, #480            @ update the buffer
    mov     r12, r1
    subs    r5, r5, #1              @ subtract 1 from the height
    bne     .wordsize               @ next iteration

.end:
    pop     {r4, r5}
    bx      lr