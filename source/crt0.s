@--------------------------------------------------------------------------------
@ crt0.s
@--------------------------------------------------------------------------------
@ Provides the first code to be executed, which loads the IWRAM/EWRAM code to
@ their appropriate places, as well as initialize the proper 
@--------------------------------------------------------------------------------
    .section .crt0, "ax"
    .align 2
    .arm
    .global _start
_start:
    b       .pastRomHeader      @ The GBA will start here, so jump to the right place
    .fill   174, 1, 0           @ The export tool will take care of the rest of the header
    .byte   0x96                @ Except for this byte right here
    .fill   10, 1, 0
    .byte   0x51                @ And this one
    .byte   0x00, 0x00

.pastRomHeader:
    mov     r0, #0x4000000      @ First, disable interrupts by the IME register
    str     r0, [r0, #0x208]    @ (here, IME = r0.0 = 0)

    @ Set up appropriate stack pointers for each mode
    mov     r0, #0x12
    msr     cpsr, r0            @ Move 0x12 to the CPSR, which sets IRQ mode
    ldr     sp, =__sp_irq       @ Set the stack pointer for IRQs
    mov     r0, #0x1F
    msr     cpsr, r0            @ Move 0x1F to the CPSR, which sets System (user) mode
    ldr     sp, =__sp_usr       @ Set the stack pointer for user mode

    adr     r0, .thumbStart+1   @ Bit 0 set to 1 to enter thumb mode
    bx      r0                  @ And branch/exchange to it

    .thumb                      @ From now on, all instructions are Thumb
.thumbStart:
    @ First, move the IWRAM subroutines to their place
    ldr     r0, =__iwram_start__
    ldr     r1, =__iwram_lma
    ldr     r2, =__iwram_end__

.iwramCopyLoop:
    ldr     r3, [r1]            @ r3 = *(u32*)r1
    str     r3, [r0]            @ *(u32*)r0 = r3
    add     r1, #4
    add     r0, #4
    cmp     r0, r2              @ compare r1 to r2
    blo     .iwramCopyLoop      @ keep copying if necessary

    @ Now that we have the IWRAM routines in place, we can actually use memcpy32 and memset32
    mov     r1, #0              @ Load the value to fill
    ldr     r2, =__bss_end__    @ Load the end of the IWRAM .bss section
    sub     r2, r0              @ The .bss section is contiguous with the IWRAM
    lsr     r2, #2              @ We need the number of words
    ldr     r3, =memset32       @ Load the memset32
    bl      .r3trampoline       @ and jump to it

    @ Now, we copy the .data section
    ldr     r1, =__data_lma     @ Load where its copy is located on ROM
    ldr     r2, =__data_end__   @ Load the end of the IWRAM .data section
    sub     r2, r0              @ The .data section is contiguous with the .bss section
    lsr     r2, #2              @ We need the number of words
    ldr     r3, =memcpy32       @ Load the memcpy32
    bl      .r3trampoline       @ and jump to it

    @ Now, we copy the .ewram section
    ldr     r0, =__ewram_start
    ldr     r1, =__ewram_lma    @ Load where its copy is located on ROM
    ldr     r2, =__ewram_end    @ Load the end of the EWRAM .ewram section (typo in the linker script probably)
    sub     r2, r0              @ Subtract to get the number of bytes to copy
    lsr     r2, #2              @ We need the number of words
    ldr     r3, =memcpy32       @ Load the memcpy32
    bl      .r3trampoline       @ memcpy32 the data

    @ And, finally, fill in the .sbss section
    mov     r1, #0              @ Fill the section with zero
    ldr     r2, =__sbss_end__   @ Load the end of the EWRAM .sbss section
    sub     r2, r0              @ The .sbss section is contiguous with the .ewram section
    lsr     r2, #2              @ We need the number of words
    ldr     r3, =memset32       @ Load the memset32
    bl      .r3trampoline       @ and jump to it

    @ Load the function that executes the global constructors
    ldr     r3, =__libc_init_array
    bl      .r3trampoline

    @ Finally, call our main (setting argc and argv to 0)
    mov     r0, #0
    mov     r1, #0
    ldr     r3, =main

    @ Trampoline through it / returning from main will be undefined behavior
.r3trampoline:
    bx      r3
