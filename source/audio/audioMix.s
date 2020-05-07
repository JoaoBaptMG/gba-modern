@--------------------------------------------------------------------------------
@ audioMix.s
@--------------------------------------------------------------------------------
@ Provides the audio mixing function
@--------------------------------------------------------------------------------

@ Pick up some constants
#include "../../build/data/audio-settings.hpp"

@ void audioMix(s16* intBuffer, s8* curMixBuffer, AudioChannel* channels)
@ r0: intBuffer, r1: curMixBuffer, r2: channels
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global audioMix
    .type audioMix STT_FUNC
audioMix:
    push    {r4-r11}

    @ First, clear the intermediate buffer memory
    mov     r3, #BUFFER_SIZE/16
    mov     r4, #0
    mov     r5, #0
    mov     r6, #0
    mov     r7, #0
    mov     r8, #0
    mov     r9, #0
    mov     r10, #0
    mov     r11, #0

.clearLoop:
    stmia   r0!, {r4-r11}           @ clear 32 bytes (worth of 16 samples)
    subs    r3, r3, #1              @ subtract 1
    bne     .clearLoop              @ 

    @ Now, for the iteration on each channel
    mov     r3, #NUM_CHANNELS

    @ restore the original buffer pointer
    subs    r0, r0, #BUFFER_SIZE*2

.forEachChannel:
    @ struct AudioChannel
    @ {
    @     const Sound* sound;
    @     u32f12 pos, inc;
    @     u32 volume;
    @ };

    @ struct Sound
    @ {
    @     u32f12 length, loopLength;
    @     // And after that comes the data in s8 chunks
    @ };

    @ load the channel data and the sound
    ldmia   r2!, {r4-r7}
    cmp     r4, #0                  @ check if the pointer actually points somewhere
    beq     .nullChannel            @ bail out if the channel doesn't exist

    ldr     r8, [r4], #8            @ load the loop size
    mov     r9, #BUFFER_SIZE        @ get the buffer size

.perSample:
    mov     r10, r5, lsr #12        
    ldrsb   r10, [r4, r10]          @ get the next sample
    mul     r10, r7, r10            @ modulate it
    ldrh    r11, [r0]               @ get the current value
    add     r11, r10, asr #6        @ add the modulated value to it
    strh    r11, [r0], #2           @ and store back to the buffer
    add     r5, r5, r6              @ add the increment
    cmp     r5, r8                  @ compare to the size
    blt     .continueProcessing     @ keep going if nothing happens
    ldr     r10, [r4, #-4]          @ load the loop length
    cmp     r10, #0                 @ check if it actually loops
    subne   r5, r5, r10             @ subtract from the actual position
    bne     .continueProcessing     @ and continue processing
    str     r10, [r2, #-16]         @ store zero to the buffer
    add     r0, r0, r9, lsl #1      @ otherwise, skip processing of the entire buffer
    sub     r0, r0, #2
    b       .finishProcessing
.continueProcessing:
    subs    r9, #1                  @ subtract one in the number of iterations
    bne     .perSample              @ and go back if there's still work to do

.finishProcessing:
    str     r5, [r2, #-12]          @ store the position back

    @ Restore the original buffer pointer
    subs    r0, r0, #BUFFER_SIZE*2

.nullChannel:
    subs    r3, #1                  @ check if there's more channels to update
    bne     .forEachChannel         @ and go back if there are

    @ Now, insert the intermediate buffer into the real buffer
    mov     r3, #BUFFER_SIZE/4
.perFinalSampleQuad:
    ldmia   r0!, {r4-r5}            @ load 4 samples at once
    and     r6, r4, #255            @ the first byte
    and     r7, r4, #255 << 16      @ the second byte
    orr     r6, r6, r7, lsr #8
    and     r7, r5, #255            @ the third byte
    and     r8, r5, #255 << 16      @ the fourth byte
    orr     r7, r7, r8, lsr #8
    orr     r6, r7, lsl #16
    str     r6, [r1], #4            @ store the byte away
    subs    r3, #1                  @ check if there are more interactions
    bne     .perFinalSampleQuad     @ go back if there are

    pop     {r4-r11}
    bx      lr