@--------------------------------------------------------------------------------
@ decodeUtf8Asm.s
@--------------------------------------------------------------------------------
@ Provides a fast way to decode a single UTF8 character
@--------------------------------------------------------------------------------

@ decodeUtf8Asm
@ inputs: r0: the pointer to the buffer
@ outputs: r0: the updated pointer to the buffer, r1: the codepoint found
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global decodeUtf8Asm
    .type decodeUtf8Asm STT_FUNC
decodeUtf8Asm:
    ldrb    r1, [r0], #1            @ load one byte of the string
    tst     r1, #0x80               @ test if it's ASCII
    bxeq    lr                      @ If so, return immediately

    ldrb    r2, [r0], #1            @ load the next byte
    and     r2, #0x3f               @ mask it
    orr     r1, r2, r1, lsl #6      @ combine with the original value to form a valid codepoint
    tst     r1, #1 << 11            @ test to see if it's a valid 2-byte codepoint
    biceq   r1, r1, #3 << 12        @ if so, mask the last bits
    bxeq    lr                      @ and return

    ldrb    r2, [r0], #1            @ load the third byte
    and     r2, #0x3f               @ mask it again
    orr     r1, r2, r1, lsl #6      @ combine with the current value to possibly form a valid codepoint        
    bx      lr                      @ assume it's a valid 3-byte codepoint (the mask was already shifted out)
