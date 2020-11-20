//--------------------------------------------------------------------------------
// context.h
//--------------------------------------------------------------------------------
// Provides an API for context switching (a primitive form of coroutines)
//--------------------------------------------------------------------------------

// IMPORTANT!!! IF YOU USE TONC'S NESTED ISR, SET THIS VALUE TO 36!!
// Otherwise, it should remain as 40
#define STACK_SIZE 40

#ifndef __ASSEMBLER__
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

// An opaque pointer (but deeply, nothing more than the sp of that context)
typedef struct __context* context_t;

// The context entry point, for beginning questions
typedef context_t (*context_entry_point_t)(context_t ctx, void* arg);

// Gives up ownership and switches to another context. This function will
// "wake up" the other context and, when this context is switched to again,
// the function will return with a new context
context_t context_switch(context_t ctx) IWRAM_CODE;

// Gives up ownership and switches to another context. This function will
// "wake up" the other context and, when this context is switched to again,
// the function will return with a new context.
// This function does the same as context_switch, but it saves a copy of
// the current context in oldCtx, it should be used for context scheduling
// from inside the IRQ
context_t context_switch2(context_t ctx, context_t* oldCtx) IWRAM_CODE;

// "Schedules" the context switch from inside the IRQ, in a way that, when
// the IRQ returns, it behaves as-if from a context_switch, and sets the
// other context's return value to emulate an IRQ return
void context_switch_from_irq(context_t ctx) IWRAM_CODE;

// All stack pointers must be aligned by 8 bytes
#define STACKPTR __attribute__((aligned(8)))

void ctx_entry_point() IWRAM_CODE;
// The stack layout after a context switch is
// (from the sp pointed by context_t) r4 r5 r6 r7 r8 r9 r10 r11 lr
// And ctx_entry_point will expect you to pass the entry point too.
// So this routine sets it to 8*(undefined) ctx_entry_point arg entry
inline static context_t context_new(void* stack_top, context_entry_point_t entry, void* arg)
{
    // Set up the stack
    unsigned int* stack = (unsigned int*)stack_top;
    stack[-1] = (unsigned int)entry; 
    stack[-2] = (unsigned int)arg;
    stack[-3] = (unsigned int)&ctx_entry_point;

    // Return the correct pointer
    return (context_t)(stack-11);
}

#ifdef __cplusplus
}
#endif
#endif