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
#include "text/mGBADebugging.hpp"
#define ASSERT(expr) if (expr); else { mgba::log(mgba::Log::Fatal, "Assertion failed: " #expr " at " __FILE__); for (;;); }
#endif
