@--------------------------------------------------------------------------------
@ audioMix.s
@--------------------------------------------------------------------------------
@ Provides the audio mixing function
@--------------------------------------------------------------------------------

@ Pick up some constants
#include "../../build/data/audio-settings.hpp"

@========================================================================================
@ Here I should explain how the audio engine is composed:
@ all the audio samples are exported as 8-bit unsigned centered-around-128 PCM;
@ this is done so the modulation functions (aka multiply-by-volume) can act on
@ two samples at a time without having to account for negatives in the middle of
@ the register (the engine "processes" two samples at a time, using a 32-bit register
@ for two "augmented" 16-bit samples. 
@
@ Since the GBA expects 8-bit signed PCM and our buffer adds modulated unsigned samples,
@ a sample bias build up and must be removed from the buffer before it is placed on the
@ final buffer where the 8-bit samples will stay. This sample bias is equal to
@ 128 * (sum of volumes of all active channels). The padding added to the end of a
@ sample must also be equal to that bias, so the wrong padding isn't added, leading to
@ clicks and pops.
@=======================================================================================

@ void audioMix(AudioChannel* channels, s8* curMixBufferLeft, s8* curMixBufferRight)
@ r0: channels, r1: curMixBufferLeft, r2: curMixBufferRight
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global audioMix
    .type audioMix STT_FUNC
audioMix:
    @ push all registers
    push    {r4-r11, lr}

    @ make space for the audio buffer and push the intermediate registers
    sub     sp, sp, #BUFFER_SIZE*4
    push    {r1, r2, r3, r4}            @ push the buffer registers, they're only used in the end
                                        @ and set space on the upper registers for the volume information
    add     r1, sp, #16                 @ set r1 to the intermediate buffer

    @ Zero out the intermediate buffer memory
    mov     r2, #BUFFER_SIZE/16         @ 32 samples per loop, 2 samples (left and right)
    mov     r3, #0 
    mov     r4, #0 
    mov     r5, #0 
    mov     r6, #0 
    mov     r7, #0 
    mov     r8, #0 
    mov     r9, #0 
    mov     r10, #0 
    stmdb   r1, {r3-r4}                 @ set the volume stack variables to zero

.zeroBuffer:
    stmia   r1!, {r3-r10}               @ 16 samples cleared
    stmia   r1!, {r3-r10}               @ 16 samples cleared = total, 32 samples
    subs    r2, r2, #1
    bne     .zeroBuffer                 @ go back if there are still samples to do

    @ restore the original pointer and prepare for the channel iterator
    mov     r2, #NUM_CHANNELS
    sub     r1, r1, #BUFFER_SIZE*4      @ restore the buffer to the beginning

    @ build the byte flag
    mov     r3, #0xFF
    orr     r3, r3, #0xFF0000

    @ Now for the main part, the iteration on the audio channels
    @ struct AudioChannel
    @ {
    @     const Sound* sound;
    @     audio::SpdType pos, playSpeed;
    @     audio::VolType volumeLeft, volumeRight;
    @ };
    @ which means a tuple (sound, pos, playSpeed, volumeLeft, volumeRight)
    @ r4: sound, r5: pos, r6: playSpeed, r7: volumeLeft, r8: volumeRight
.forEachChannel:
    ldmia   r0!, {r4-r8}                @ load the audio channel tuple into r4 to r8
    cmp     r4, #0                      @ check if the channel is active
    beq     .nullChannel                @ and go back if it's not

    @ check null-volume sounds as well
    cmp     r7, #0
    cmpeq   r8, #0
    beq     .nullChannel

    ldmdb   r1, {r9, r10}               @ load the volume sums in the stack
    add     r9, r9, r7                  @ add the left volume
    add     r10, r10, r8                @ add the right volume
    stmdb   r1, {r9, r10}               @ store the sums back in the stack

    cmp     r6, #1 << 12                @ check if the play speed is exactly 1
    bne     .bendPath                 @ and go to the non-optimized path if that's the case

    @ struct Sound
    @ {
    @     u32 length, loopLength;
    @     // And after that comes the data in u8 chunks
    @ };
    @ so, a first tuple (length, loopLength) and then the data
    ldr     r12, [r4], #8               @ load the length
    sub     lr, r12, r5, lsr #12        @ get the number of samples to play
    cmp     lr, #BUFFER_SIZE            @ compare to the size of the buffer
    movhi   lr, #BUFFER_SIZE            @ move the maximum size
    orrls   r0, r0, #1 << 31            @ mark a flag if the end of the sound is reached
    push    {r4, r5, lr}                @ push required variables
    add     r4, r4, r5, lsr #12         @ add the position to the pointer

    tst     r5, #3 << 12                @ check for unaligned position
    bne     .unalignedFastLoad          @ go for the semi-optimised unaligned version

    @ The intermediate buffer is interleaved, meaning that, for each 4 halfwords, we have
    @ bufferL[0], bufferL[2], bufferL[1], bufferL[3],
    @ bufferR[0], bufferR[2], bufferR[1], bufferR[3],
.perSampleQuad:
    ldr     r5, [r4], #4                @ load four samples
    and     r6, r3, r5, lsr #8          @ mask the odd samples
    and     r5, r3, r5                  @ mask the even samples
    mul     r9, r5, r7                  @ modulate for the left channel
    mul     r10, r6, r7                 @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the left buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the left buffer back
    mul     r9, r5, r8                  @ modulate for the right channel
    mul     r10, r6, r8                 @ modulate the even samples
    ldmia   r1, {r11-r12}               @ load the right buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the right buffer back
    subs    lr, lr, #4                  @ decrease the counter
    bne     .perSampleQuad

    pop     {r4, r5, lr}                @ restore the original pointer, position and number written to buffer
.fastAfterProcessing:
    add     r5, r5, lr, lsl #12         @ add the buffer size regardless
    tst     r0, #1 << 31                @ check if the sound's end was detected
    beq     .finishProcessing           @ move on if it was not

    @ If the channel came to the end of the sound, either stop the sound or subtract the length
    bic     r0, r0, #1 << 31            @ unset the flag
    ldr     r12, [r4, #-4]              @ load the length into r12
    cmp     r12, #0                     @ check if it's zero
    beq     .stopChannel                @ stop the channel

    sub     r5, r5, r12, lsl #12        @ subtract the loop length
    rsbs    lr, lr, #BUFFER_SIZE        @ get the remaining buffer data to be sent
    beq     .finishProcessing           @ bail out if there is no data left to be sent
    push    {r4, r5, lr}                @ push the required variables again
    add     r4, r4, r5, lsr #12         @ get the up-to-date pointer to the data
    tst     r5, #3 << 12                @ check for unaligned position
    bne     .unalignedFastLoad          @ go for the semi-optimised unaligned version
    b       .perSampleQuad              @ send the remaining data if there is still data to be sent

.stopChannel:
    str     r12, [r0, #-20]             @ store a zero to the channel pointer
    rsbs    lr, lr, #BUFFER_SIZE        @ get the remaining buffer data to be sent
    beq     .finishProcessing           @ bail out if there is no data left to be sent

.toPadBuffer:
    orr     r7, r7, r7, lsl #16         @ replicate the volume pointers, left
    orr     r8, r8, r8, lsl #16         @ and right

.padBuffer:
    ldmia   r1, {r9-r12}                @ load four left and four right samples
    add     r9, r9, r7, lsl #7          @ add the padding offset to the even left samples
    add     r10, r10, r7, lsl #7        @ to the odd left samples
    add     r11, r11, r8, lsl #7        @ to the even right samples
    add     r12, r12, r8, lsl #7        @ to the odd right samples
    stmia   r1!, {r9-r12}               @ store the samples in the buffer
    subs    lr, lr, #4                  @ decrease the counter
    bne     .padBuffer                  @ and go back if required

.finishProcessing:
    str     r5, [r0, #-16]              @ store the position back in the audio channel
    sub     r1, r1, #BUFFER_SIZE*4      @ rewind the buffer

.nullChannel:
    subs    r2, r2, #1                  @ decrease the channel counter
    bne     .forEachChannel             @ go back if it's not over yet

    @ So far, the intermediate buffer encoded in 12-bit unsigned PCM, so we need to
    @ remove the cumulative volume in order to convert it to signed PCM
    pop     {r1, r2, r4, r5}            @ pop the destination registers and the volume values
    orr     r4, r4, r4, lsl #16         @ mirror the left volume register
    orr     r5, r5, r5, lsl #16         @ mirror the right volume register

    @ Now, insert the intermediate buffer into the real buffer
    mov     r0, #BUFFER_SIZE/4
.finalSampleProcessing:
    pop     {r6-r9}                     @ load 4 samples left and right (they're interleaved)
    sub     r6, r6, r4, lsl #7          @ subtract the channel bias from the even left samples
    sub     r7, r7, r4, lsl #7          @ subtract the channel bias from the odd left samples
    sub     r8, r8, r5, lsl #7          @ subtract the channel bias from the even right samples
    sub     r9, r9, r5, lsl #7          @ subtract the channel bias from the odd right samples
    and     r6, r3, r6, lsr #4          @ modulate the volume and wrap it (even left samples)
    and     r7, r3, r7, lsr #4          @ modulate the volume and wrap it (odd left samples)
    and     r8, r3, r8, lsr #4          @ modulate the volume and wrap it (even right samples)
    and     r9, r3, r9, lsr #4          @ modulate the volume and wrap it (odd right samples)
    orr     r6, r6, r7, lsl #8          @ compose the even and odd samples in the left
    orr     r8, r8, r9, lsl #8          @ compose the even and odd samples in the right
    str     r6, [r1], #4                @ store to the left buffer
    str     r8, [r2], #4                @ store to the right buffer
    subs    r0, r0, #1                  @ decrease the counter
    bne     .finalSampleProcessing      @ and go back if needed

    @ pop all registers and return
    pop     {r4-r11, lr}
    bx      lr

.unalignedFastLoad:
    push    {r0, r2}                    @ save two more registers for the task
    and     r5, r4, #3                  @ get the "unalignment measure" off the bank
    bic     r4, r4, #3                  @ and erase it from the pointer

    @ Now, we are going to do some self-modifying code trickery here
    @ We are going to modify the second byte of the instruction
    @ "mov r0, r2, lsr #8". In this byte, the upper 4 bits represent
    @ the destination register and the lower 4 bits are the upper 4 bits
    @ (out of a 5-bit shift bleeding into the first byte); so if we set
    @ this byte to shift<<2, we're essentially modifying the instruction to
    @ mean "mov r0, r2, lsr #(shift<<3)"; which can be used to dynamically
    @ alter the instruction according to the "unalignment measure"
    mov     r5, r5, lsl #2
    strb    r5, .firstModifyPoint+1
    strb    r5, .secondModifyPoint+5

    @ The second code injection is in the "orr r5, r0, r2, lsl #4"
    @ if we set the second byte to 0x50 + (16 - (shift<<2)), we change
    @ it to "orr r5, r0, r2, lsl #(32 - (shift<<3))", to cater for the
    @ "unalignment measure".
    rsb     r5, r5, #0x60               @ set the correct bits in the instruction
    strb    r5, .secondModifyPoint+1

    ldr     r2, [r4]                    @ load a word
.firstModifyPoint:
    mov     r0, r2, lsr #8              @ move the rest of the word to the bank

.perSampleQuadUnaligned:
    ldr     r2, [r4, #4]!               @ load the next word
.secondModifyPoint:
    orr     r5, r0, r2, lsl #24         @ combine the previous bank and the current word
    mov     r0, r2, lsr #8              @ move the rest of the word to the bank

    @ do the rest of the processing
.recoverFromUnalignedPadding:
    and     r6, r3, r5, lsr #8          @ mask the odd samples
    and     r5, r3, r5                  @ mask the even samples
    mul     r9, r5, r7                  @ modulate for the left channel
    mul     r10, r6, r7                 @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the left buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the left buffer back
    mul     r9, r5, r8                  @ modulate for the right channel
    mul     r10, r6, r8                 @ modulate the even samples
    ldmia   r1, {r11-r12}               @ load the right buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the right buffer back
    subs    lr, lr, #4                  @ decrease the counter
    bhi     .perSampleQuadUnaligned
    popeq   {r0, r2, r4, r5, lr}        @ pop everything
    beq     .fastAfterProcessing        @ the rest is just machinery

.unalignedPadding:
    @ special case for when the counter dips below zero
    @ the only way we an arrive here is if a looping sample goes to the
    @ unaligned branch (because all sound data is aligned), so it must be the case
    @ that there's a loop length. Let's go get it
    sub     r1, r1, #16                 @ rewind a single chunk
    ldr     r12, [sp, #8]               @ load the previous pointer
    ldr     r12, [r12, #-4]             @ load the loop length into r12
    ldr     r6, [sp, #16]               @ get the lr and "hijack" it
    sub     r6, r6, lr                  @ subtract the original lr
    str     r6, [sp, #16]               @ and put it back in place
    add     lr, lr, #1                  @ branch based on lr, so this is to keep the branches safe
    sub     pc, pc, lr, lsl #4          @ and now branch based on a decision
    mov     r0, r0                      @ to clear the pipeline

    @ first, when lr = -1
    ldr     r2, [r4, #-4]               @ load a word
    mov     r5, r2, lsr #8              @ store the three last bytes
    sub     r4, r4, r12                 @ subtract the current pointer by the loop length
    b       .fillInTheRest1             @ fill in what is needed

    @ now, lr = -2
    ldrh    r5, [r4, #-2]               @ load a halfword
    sub     r4, r4, r12                 @ subtract the current pointer by the loop length
    b       .fillInTheRest2             @ fill in what is needed
    mov     r0, r0                      @ padding

    @ finally, lr = -3
    ldrb    r5, [r4, #-1]               @ load a byte
    sub     r4, r4, r12                 @ subtract the current pointer by the loop length

.fillInTheRest3:
    ldrb    r2, [r4], #1                    @ load a single byte
    orr     r5, r5, r2, lsl #8              @ place it on the correct place
.fillInTheRest2:
    ldrb    r2, [r4], #1                    @ load a single byte
    orr     r5, r5, r2, lsl #16             @ place it on the correct place
.fillInTheRest1:
    ldrb    r2, [r4], #1                    @ load a single byte
    orr     r5, r5, r2, lsl #24             @ place it on the correct place
    mov     lr, #4                          @ set a last place
    b       .recoverFromUnalignedPadding    @ and branch

.bendPath:
    @ Here we have:
    @ r0: AudioChannel pointer, r1: intermediate buffer, r2: channel count, r3: 0x00FF00FF
    @ r4: sound, r5: pos, r6: playSpeed, r7: volumeLeft, r8: volumeRight
    push    {r0, r2}                    @ push some registers
    mov     lr, #BUFFER_SIZE            @ move the number of samples to process
    ldmia   r4!, {r0, r2}               @ song length and loop length
    mla     r9, r6, lr, r5              @ get the final position
    cmp     r9, r0, lsl #12             @ check if it's less than the length
    bhi     .bendPerSampleQuad          @ if so, we can skip the overflow checks

.noCheckBendPerSampleQuad:
    ldrb    r0, [r4, r5, lsr #12]       @ load the first sample
    add     r5, r5, r6                  @ add the playing speed
    ldrb    r2, [r4, r5, lsr #12]       @ load the second sample
    add     r5, r5, r6                  @ add the playing speed
    ldrb    r9, [r4, r5, lsr #12]       @ load the third sample
    orr     r0, r0, r9, lsl #16         @ put it on its place
    add     r5, r5, r6                  @ add the playing speed
    ldrb    r9, [r4, r5, lsr #12]       @ load the final sample
    orr     r2, r2, r9, lsl #16         @ put it on its place
    add     r5, r5, r6                  @ add the playing speed  

    @ now do the processing
    mul     r9, r0, r7                  @ modulate for the left channel
    mul     r10, r2, r7                 @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the left buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the left buffer back
    mul     r9, r0, r8                  @ modulate for the right channel
    mul     r10, r2, r8                 @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the right buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the right buffer back
    subs    lr, lr, #4                  @ decrease the counter
    bne     .noCheckBendPerSampleQuad

.noCheckAfterProcessing:
    @ r5 will have the "correct" position, just check if we need to update it
    pop     {r0, r2}                    @ pop the other variables
    ldmdb   r4, {r11-r12}               @ load the length and loop length again
    cmp     r5, r11, lsl #12            @ compare with the length
    blo     .finishProcessing           @ store it back into the right position

    cmp     r12, #0                     @ check if the loop length is nonzero
    streq   r12, [r0, #-20]             @ set the channel to zero
    sub     r5, r5, r12, lsl #12        @ subtract the loop lngth
    b       .finishProcessing           @ and finish processing

.bendPerSampleQuad:
    @ this one is tricky because we have a lot of stuff going on here
    ldrb    r11, [r4, r5, lsr #12]      @ load the first sample
    add     r5, r5, r6                  @ add the playing speed
    cmp     r5, r0, lsl #12             @ comapre to the length
    bhs     .handleOverflow3            @ and handle overflow
.loadNext3:
    ldrb    r12, [r4, r5, lsr #12]      @ load the second sample
    add     r5, r5, r6                  @ add the playing speed
    cmp     r5, r0, lsl #12             @ comapre to the length
    bhs     .handleOverflow2            @ and handle overflow
.loadNext2:
    ldrb    r9, [r4, r5, lsr #12]       @ load the third sample
    orr     r11, r11, r9, lsl #16       @ put it on its place
    add     r5, r5, r6                  @ add the playing speed
    cmp     r5, r0, lsl #12             @ comapre to the length
    bhs     .handleOverflow1            @ and handle overflow
.loadNext1:
    ldrb    r9, [r4, r5, lsr #12]       @ load the final sample
    orr     r12, r12, r9, lsl #16       @ put it on its place
    add     r5, r5, r6                  @ add the playing speed 

    @ now do the processing
    push    {r11-r12}                   @ push them into stack
    mul     r9, r11, r7                 @ modulate for the left channel
    mul     r10, r12, r7                @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the left buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the left buffer back
    pop     {r11-r12}                   @ restore the old values
    mul     r9, r11, r8                 @ modulate for the right channel
    mul     r10, r12, r8                @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the right buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the right buffer back
    subs    lr, lr, #4                  @ decrease the counter
    bne     .bendPerSampleQuad
    b       .noCheckAfterProcessing

    @ special overflow cases
.handleOverflow3:
    cmp     r2, #0                      @ if the loop length is zero
    subne   r5, r5, r2, lsl #12         @ just subtract it
    bne     .loadNext3                  @ and go back
    orr     r11, r11, #1 << 23          @ append a blank sample
    mov     r12, #1 << 7                @ append a blank sample
    orr     r12, r12, #1 << 23          @ append a blank sample
    b       .bendOverflow               @ handle the overflow

.handleOverflow2:
    @ special overflow cases
    cmp     r2, #0                      @ if the loop length is zero
    subne   r5, r5, r2, lsl #12         @ just subtract it
    bne     .loadNext2                  @ and go back
    orr     r11, r11, #1 << 23          @ append a blank sample
    orr     r12, r12, #1 << 23          @ append a blank sample
    b       .bendOverflow               @ handle the overflow

.handleOverflow1:
    @ special overflow cases
    cmp     r2, #0                      @ if the loop length is zero
    subne   r5, r5, r2, lsl #12         @ just subtract it
    bne     .loadNext1                  @ and go back
    orr     r12, r12, #1 << 23          @ append a blank sample

.bendOverflow:
    @ same processing case, but go to the pad buffer instead
    push    {r11-r12}                   @ push them into stack
    mul     r9, r11, r7                 @ modulate for the left channel
    mul     r10, r12, r7                @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the left buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the left buffer back
    pop     {r11-r12}                   @ restore the old values
    mul     r9, r11, r8                 @ modulate for the right channel
    mul     r10, r12, r8                @ modulate the odd samples
    ldmia   r1, {r11-r12}               @ load the right buffer
    add     r11, r11, r9                @ add the even samples
    add     r12, r12, r10               @ add the odd samples
    stmia   r1!, {r11-r12}              @ store the right buffer back
    pop     {r0, r2}                    @ pop the registers
    mov     r12, #0                     @ move zero to r12
    str     r12, [r0, #-20]             @ zero out the pointer
    subs    lr, lr, #4                  @ decrease the counter
    beq     .finishProcessing
    b       .toPadBuffer                @ and go to the pad buffer routine
