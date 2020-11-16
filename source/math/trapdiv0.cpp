//--------------------------------------------------------------------------------
// trapdiv0.c
//--------------------------------------------------------------------------------
// Provides a trap for when a division by 0 is detected
//--------------------------------------------------------------------------------
#include "text/mGBADebugging.hpp"

extern "C"
{
    [[noreturn]] int __aeabi_idiv0()
    {
        mgba::log(mgba::Log::Fatal, "Division by zero detected!");
        for (;;);
    }
}