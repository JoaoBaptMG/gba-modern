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

@
@ to look up a font interval, we do a linear search, it is efficient enough
@

@ void tile4bppPutString(int x, int y, const char* str, int size, const Font* font, COLOR color, void* buffer, u32 pixelHeight);
@ x: r0, y: r1, str: r2, size: r3, font: sp[0], color: sp[4], buffer: sp[8], pixelHeight: sp[12]
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global tile4bppPutString
    .type tile4bppPutString STT_FUNC
tile4bppPutString:
    push    {r3-r11}                @ Save the registers we're going to use (sp goes down by 36)

    @ Allocate enough memory space to look up for up to size characters
    sub     r12, sp, r3, lsl #1     @ one word per codepoint
    sub     r12, r12, #4            @ plus two more words for a sentinel

    @ Now, UTF-8 unpacking; this can be really wasteful if not done correctly
.charLoadLoop:
    @ First, check for the size
    subs    r3, r3, #1              @ decrease one in the size
    blo     .charLoadLoopEnd        @ finished unpacking all characters
    
    ldrb    r4, [r2], #1            @ load one byte of the string
    tst     r4, #0x80               @ test if it's ASCII
    streqh  r4, [r12], #2           @ If so, store it immediately and bail out
    beq     .charLoadLoop

    sub     r3, r3, #1              @ decrease one in the size
    ldrb    r5, [r2], #1           @ load the next byte
    and     r5, #0x3f               @ mask it
    orr     r4, r5, r4, lsl #6      @ combine with the original value to form a valid codepoint
    tst     r4, #1 << 11            @ test to see if it's a valid 2-byte codepoint
    biceq   r4, r4, #3 << 12        @ if so, mask the last bits
    streqh  r4, [r12], #2           @ store it into the character
    beq     .charLoadLoop           @ and go back to the loop

    sub     r3, r3, #1              @ decrease one in the size
    ldrb    r5, [r2], #1            @ load the third byte
    and     r5, #0x3f               @ mask it again
    orr     r4, r5, r4, lsl #6      @ combine with the current value to possibly form a valid codepoint        
    strh    r4, [r12], #2           @ assume it's a valid 3-byte codepoint (the mask was already shifted out)
    b       .charLoadLoop

.charLoadLoopEnd:
    @ Put a 0 as a terminator to this location
    mov     r3, #0
    strh    r3, [r12]

    ldr     r4, [sp, #36]           @ Load the font pointer into r4
    ldr     r3, [sp]                @ restore the original size
    sub     r12, sp, r3, lsl #1     @ and go back to the beginning of the string
    sub     r12, r12, #4            @ plus two more words for the sentinel

    @ r4: font pointer
    @ We don't need the original string (r2) anymore
    @ Here, we unpack the font glyph addresses, and compute the minimum and maximum ascents and descents
    mvn     r8, #0x80000000         @ r8 = minimum span
    mov     r9, #0x80000000         @ r9 = maximum span
    mov     r10, #0                 @ r10 = maximum x advance

.glyphUnpackLoop:
    ldrh    r2, [r12]               @ r2 = current codepoint
    cmp     r2, #0                  @ check if it's zero
    beq     .glpyhUnpackLoopEnd     @ bug out if so

    mov     r5, #0                  @ store the offset away
    add     r6, r4, #8              @ r6 points to &(font->glyphIntervals[0]) now 

.singleGlyphLoop:
    ldr     r3, [r6], #4            @ r3 points to the next glyph interval
    cmp     r3, #0                  @ bail out if r3 is the sentinel value
    streqh  r3, [r12], #2           @ write 0 to the index
    beq     .glyphUnpackLoop

    cmp     r2, r3, lsr #16         @ compare the character with the glyph interval end
    mov     r7, r3, lsl #16         @ mask out the end
    bhs     .updateOffset           @ Go to the next interval if the glyph has not ended
    subs    r2, r2, r7, lsr #16     @ And check if the glyph is within the interval
    addhs   r2, r2, r5              @ add the offset
    strhsh  r2, [r12], #2           @ and write the glyph index to the offset
    bhs     .computeMinMax          @ going back to compute the minimum and maximum span

.updateOffset:
    add     r5, r5, r3, lsr #16     @ If not, update the offset (glyph end - glyph begin)
    sub     r5, r5, r7, lsr #16
    b       .singleGlyphLoop

.computeMinMax:
    ldr     r6, [r4]                @ Load the font glyph pointer
    ldr     r3, [r6, r2, lsl #3]    @ Load the config word
    mov     r5, r3, lsl #8          @ Last byte will contain gl->offsetY
    cmp     r8, r5, asr #24         @ Compare r8 with sign-extended gl->offsetY
    movgt   r8, r5, asr #24         @ If it's greater move to it
    mov     r6, r3, lsl #1          @ Get the glyph's x advance
    add     r10, r10, r6, lsr #24   @ And add to the total x advance
    and     r3, r3, #0xFF           @ Mask the height
    add     r3, r5, asr #24         @ ...and add to the offsetY to get the maximum span
    cmp     r9, r3                  @ compare r9
    movlt   r9, r3                  @ If it's less, move to it
    b .glyphUnpackLoop              @ ...keep going

.glpyhUnpackLoopEnd:
    @ Now, the codepoints are transformed into glyphs, and sp points to one past the end
    @ of the string. r2, r3, r5, r6, r7 are free, r8 and r9 are the minimum and maximum spans
    sub     r2, sp, r12             @ Get the number of bytes "remaining"
    ldr     r3, [sp]                @ Get the byte size of the string
    sub     r2, r3, r2, lsr #1      @ Get the actual number of characters in the string
    add     r2, r2, #2              @ Add 2 to compensate for the sp padding up there
    and     r7, r0, #7              @ Get the x offset inside a tile
    add     r10, r10, r7            @ Add to the maximum x advance of the string
    add     r10, r10, #31           @ Pad it to the next multiple of 32
    bic     r10, r10, #31           @ ...
    sub     r9, r9, r8              @ Get the maximum height of the string
    mov     r10, r10, lsr #3        @ Find out how many bytes the bitmask will occupy
    mul     r5, r10, r9             @ And multiply by the height of the mask
    sub     r12, sp, r3, lsl #1     @ Point r12 back to the beginning of the code points
    sub     r12, r12, #4            @ Compensate for the padding
    sub     r11, r12, r5            @ And allocate space for the bitmask
    bic     r11, r11, #3            @ Align it to a 4-byte boundary

    @ Clear all the bitmask
    mov     r6, #0
    sub     r12, r12, #4            @ Subtract 4 to prevent overwriting the codepoint buffer
.clearMask:
    str     r6, [r11], #4           @ Move a 0 to the location
    cmp     r11, r12                @ Compare to the mask size
    blt     .clearMask              @ Keep clearing
    add     r12, r12, #4            @ Add 4 again
    sub     r11, r12, r5            @ And go back to the beginning of the bitmask
    bic     r11, r11, #3            @ Aligned to a 4-byte boundary

    @ So far:
    @ r0 = offset x, r1 = offset y, r2 = glyph size, r3 = byte size
    @ r4 = Font pointer, r5 = mask total size, r6 = zero, r7 = offset x & 7
    @ r8 = minimum ascent, r9 = mask height, r10 = mask width (in bytes), 
    @ r11 = pointer to mask, r12 = pointer to codepoints

    @ We don't need byte size here now
    ldr     r3, [r4]                @ Load the glyph data pointer into r3
    str     r0, [r11, #-4]          @ Store the x offset behind the data
    str     r1, [sp]                @ Store the y offset in the stack
    str     r9, [sp, #-4]           

.pasteCharacter:
    subs    r2, r2, #1              @ Decrease the glyph size...
    blo     .pasteCharacterEnd      @ ...and bail out if we have finished
    ldrh    r4, [r12], #2           @ Load next glyph
    add     r4, r3, r4, lsl #3      @ Get the pointer into the glyph
    ldm     r4, {r4, r5}            @ And load the config words
    mov     r9, r4, lsl #1          @ Load the x advance into r9
    tst     r4, #0xFF               @ Test if we have zero height
    addeq   r7, r7, r9, lsr #25     @ If so, just advance the x...
    beq     .pasteCharacter         @ ... and branch back

    mov     r6, r4, lsl #8          @ Load y offset in r6
    rsb     r6, r8, r6, asr #24     @ Get the offset into the max (y offset - minimum ascent)
    mla     r6, r10, r6, r11        @ Get the pointer into the bitmask for the expected line
    mov     r0, r4, lsl #16         @ Load x offset in r0
    add     r0, r7, r0, asr #24     @ And add to the offset
    add     r6, r6, r0, lsr #3      @ Offset the pointer
    bic     r6, r6, #3              @ Make sure it is correctly aligned
    and     r0, r0, #31             @ And the bit offset into the word

    tst     r4, #1 << 31            @ Test the attrib flag to see if it's a halfword or a word
    and     r4, r4, #0xFF           @ Keep only the height
    bne     .pasteWord              @ If it is a word, go to the right place
    @ Check if the halfword would need to be split between words
    add     r7, r7, r9, lsr #25     @ Add the current offset to the advance
    add     r0, r0, r9, lsr #25     @ Add the current offset to the advance
    cmp     r0, #32                 @ By checking if current x offset into word + x advance is >= 32
    sub     r0, r0, r9, lsr #25     @ Subtract the thing back
    bhs     .pasteSplitHalfWord

.pasteWholeHalfword:
    ldrh    r1, [r5], #2            @ Load a single halfword
    ldr     r9, [r6]                @ Load the current mask word
    orr     r9, r9, r1, lsl r0      @ Paste the mask into the correct place
    str     r9, [r6], r10           @ And store the word again, advancing the pointer
    subs    r4, r4, #1              @ Decrease one in the height
    bne     .pasteWholeHalfword     @ And go back to the process
    b       .pasteCharacter         @ Finally, pick up the next character

.pasteSplitHalfWord:
    ldrh    r1, [r5], #2            @ Load a single halfword
    rsb     r0, r0, #32             @ Take the complementary shift
    ldr     r9, [r6, #4]            @ Load the next mask word
    orr     r9, r9, r1, lsr r0      @ Paste the mask into the correct place
    str     r9, [r6, #4]            @ And store the word again
    rsb     r0, r0, #32             @ Shift it back to normal
    ldr     r9, [r6]                @ Load the current mask word
    orr     r9, r9, r1, lsl r0      @ Paste the mask into the correct place
    str     r9, [r6], r10           @ And store the word again, advancing the pointer
    subs    r4, r4, #1              @ Decrease one in the height
    bne     .pasteSplitHalfWord     @ And go back to the process
    b       .pasteCharacter         @ Finally, pick up the next character          

.pasteWord:
    mov     r9, r9, lsl #1          @ Take out the flag pointer
    add     r7, r0, r9, lsr #25     @ And add the current offset to the advance

.pasteCurWord:
    ldr     r1, [r5], #4            @ Load a single halfword
    rsb     r0, r0, #32             @ Take the complementary shift
    ldr     r9, [r6, #4]            @ Load the next mask word
    orr     r9, r9, r1, lsr r0      @ Paste the mask into the correct place
    str     r9, [r6, #4]            @ And store the word again
    rsb     r0, r0, #32             @ Shift it back to normal
    ldr     r9, [r6]                @ Load the current mask word
    orr     r9, r9, r1, lsl r0      @ Paste the mask into the correct place
    str     r9, [r6], r10           @ And store the word again, advancing the pointer
    subs    r4, r4, #1              @ Decrease one in the height
    bne     .pasteCurWord           @ And go back to the process
    b       .pasteCharacter         @ Finally, pick up the next character      

.pasteCharacterEnd:
    @ Now the mask is finally prepared, we can put it into the VRAM
    @ Restore the informations about the offset y and the mask height
    ldr     r0, [r11, #-4]
    ldr     r1, [sp]
    ldr     r9, [sp, #-4]

    @ Here, we have
    @ r0 = offset x, r1 = offset y, r2 = -1, r3 = glpyh data (not needed anymore)
    @ r4 = 0, r5 free, r6 = last mask pointer (not needed), r7 = scratched
    @ r8 = minimum ascent, r9 = mask height, r10 = mask width (in bytes)
    @ r11 = pointer to mask, r12 = scratched
    @ Now, load all the remaining things needed: color, buffer and pixel height
    add     r5, sp, #40             @ Make r5 point to where we want to
    ldm     r5, {r5-r7}             @ Load r5: color, r6: buffer, r7: pixel height
    ldr     r3, =0x01010101         @ Preload the auxiliary mask to be used down there

    @ Separate the X coordinate into its quotient and remainder by 8
    mov     r0, r0, lsr #3          @ now r0 = x/8

    add     r1, r1, r8              @ Add the minimum ascent
    mla     r1, r0, r7, r1          @ y + pixelHeight*tileX
    add     r12, r6, r1, lsl #2     @ now r12 is &buffer[y + pixelHeight*tileX]

    @ Move it to the right location
    mov     r2, r10                 @ Move the width information to r2 as a counter
.unpackMaskToVRAM:
    mov     r6, r12                 @ A copy of the buffer to r6
    mov     r4, r11                 @ Put a copy of the mask array into r4
    mov     r8, r9                  @ r8 will also act as a counter, of the pixel height

.copyMask:
    ldrb    r0, [r4], r10           @ Load one byte of the mask buffer
    orr     r0, r0, r0, lsl #12     @ Shift the higher nibbles into their place...
    orr     r0, r0, r0, lsl #6      @ ...in order to be able to apply the mask
    and     r1, r0, r3, lsl #1      @ Pick up the even bits
    and     r0, r0, r3              @ Pick up the odd bits
    orr     r0, r0, r1, lsl #3      @ Combine them into a single 4-bit unpacked value 
    mul     r0, r5, r0              @ Multiply by the color
    str     r0, [r6], #4            @ Store it into the buffer
    subs    r8, r8, #1              @ Decrease the height
    bne     .copyMask

    add     r11, r11, #1            @ Add one byte to the mask buffer
    add     r12, r12, r7, lsl #2    @ Advance by the pixel height (x4 to get the byte height)
    subs    r2, r2, #1              @ Subtract one of the width...
    bne     .unpackMaskToVRAM       @ ... and keep going while necessary

.end:
    pop     {r3-r11}                @ restore the registers
    bx      lr
