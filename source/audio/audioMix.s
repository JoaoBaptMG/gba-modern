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

@ void audioMix(s16* intBuffer, s8* curMixBuffer, AudioChannel* channels)
@ r0: intBuffer, r1: curMixBuffer, r2: channels
    .section .iwram, "ax", %progbits
    .align 2
    .arm
    .global audioMix
    .type audioMix STT_FUNC
audioMix:
    push    {r4-r11, lr}

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
    mov     lr, #0                  @ as an accumulative volume counter

    @ restore the original buffer pointer
    subs    r0, r0, #BUFFER_SIZE*2

    @ load the 0x00FF00FF mask on r12
    mov     r12, #0xFF
    orr     r12, r12, #0xFF << 16

.forEachChannel:
    @ struct AudioChannel
    @ {
    @     const Sound* sound;
    @     u32 pos, volume;
    @ };

    @ struct Sound
    @ {
    @     u32 length, loopLength;
    @     // And after that comes the data in u8 chunks
    @ };

    @ load the channel data and the sound
    ldmia   r2!, {r4-r6}
    cmp     r4, #0                  @ check if the pointer actually points somewhere
    beq     .nullChannel            @ bail out if the channel doesn't exist

    ldrh    r7, [r4], #8            @ load the sound size
    sub     r7, r7, r5              @ get the number of samples still there
    cmp     r7, #BUFFER_SIZE        @ compare to the actual buffer size
    movgt   r7, #BUFFER_SIZE        @ now r7 is the minimum between them
    orrle   r2, #1 << 31            @ set a flag on r2 (it's a pointer, it's free) to go back
    add     r4, r4, r5              @ add the position
    add     lr, lr, r6, lsl #7      @ to account for the unsigned compensation later

    @ The intermediate buffer is interleaved, meaning that, for each 4 halfwords, we have
    @ buffer[0], buffer[2], buffer[1], buffer[3]. This is used to improve the speed
.perSampleQuad:
    ldr     r8, [r4], #4            @ load a sample of four chunks
    and     r9, r12, r8, lsr #8     @ mask the odd samples
    and     r8, r12, r8             @ mask the even samples
    mul     r10, r8, r6             @ modulate the even samples
    mul     r11, r9, r6             @ modulate the odd samples
    ldmia   r0, {r8-r9}             @ load the 8-word data
    add     r8, r8, r10             @ add the even samples
    add     r9, r9, r11             @ add the odd samples
    stmia   r0!, {r8-r9}            @ put everything back
    subs    r7, r7, #4              @ decrease the counter
    bne     .perSampleQuad          @ and go back to processing the samples

    tst     r2, #1 << 31            @ check if processing should continue
    addeq   r5, r5, #BUFFER_SIZE    @ if not, add the buffer size to the position
    beq     .finishProcessing       @ and just finish the processing

    bic     r2, #1 << 31            @ clear the flag
    ldr     r4, [r2, #-12]          @ reload the pointer
    ldm     r4, {r4, r8}            @ load the loop start and length
    sub     r7, r4, r5              @ get the number of samples still there
    rsb     r7, r7, #BUFFER_SIZE    @ get the remaining of samples
    cmp     r8, #0                  @ if the loop length is something...
    subne   r5, r4, r8              @ subtract the loop length
    bne     .perSampleQuad          @ and go back to the sample chunk

    str     r8, [r2, #-12]          @ store a zero to the channel pointer

    @ We need to pad the buffer with 128*vol, to compensate for the channel's unsignedness
    mov     r10, r6, lsl #7          @ multiply by 128
    orr     r10, r10, r10, lsl #16   @ get the new sample
    mov     r11, r11                 @ store a copy on r11

.padBuffer:
    ldmia   r0, {r8-r9}             @ load the 8-word data
    add     r8, r8, r10             @ add the even samples
    add     r9, r9, r11             @ add the odd samples
    stmia   r0!, {r8-r9}            @ put everything back
    subs    r7, r7, #4              @ decrease the sample counter
    bne     .padBuffer              @ keep going if necessary

.finishProcessing:
    str     r5, [r2, #-8]           @ store the position back

    @ Restore the original buffer pointer
    subs    r0, r0, #BUFFER_SIZE*2

.nullChannel:
    subs    r3, #1                  @ check if there's more channels to update
    bne     .forEachChannel         @ and go back if there are

    @ So far, the intermediate buffer encoded in 12-bit unsigned PCM, so we need to
    @ remove (1 << 7) times the cumulative volume in order to convert it to signed PCM
    orr     lr, lr, lr, lsl #16         @ and into the highest halfword

    @ Now, insert the intermediate buffer into the real buffer
    mov     r3, #BUFFER_SIZE/4
.perFinalSampleQuad:
    ldmia   r0!, {r4-r5}            @ load 4 samples at once (they're interleaved)
    sub     r4, r4, lr              @ subtract the channel bias
    sub     r5, r5, lr              @ subtract the channel bias
    and     r4, r12, r4, lsr #4     @ modulate the volume and wrap it back
    and     r5, r12, r5, lsr #4     @ do the same for the odd samples
    orr     r4, r4, r5, lsl #8      @ compose the samples again
    str     r5, [r1], #4            @ store them away
    subs    r3, #1                  @ check if there are more interactions
    bne     .perFinalSampleQuad     @ go back if there are

    pop     {r4-r11, lr}
    bx      lr