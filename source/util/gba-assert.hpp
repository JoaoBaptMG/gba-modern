//--------------------------------------------------------------------------------
// gba-assert.hpp
//--------------------------------------------------------------------------------
// Provides an ASSERT macro that NO$GBA catches and breaks on
//--------------------------------------------------------------------------------
#pragma once

//#define DISABLE_ASSERT

#ifdef DISABLE_ASSERT
#define ASSERT(expr)
#else
#define ASSERT(expr) if (expr); else asm volatile("mov r11, r11" ::: "r11")
#endif
