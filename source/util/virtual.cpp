//--------------------------------------------------------------------------------
// virtual.cpp
//--------------------------------------------------------------------------------
// Prevents the inclusion of expensive exception mechanisms in presence of
// pure virtual function declarations, as well as providing a simpler
// exit handler to cleanup the code
//--------------------------------------------------------------------------------
#include "gba-assert.hpp"

void __cxa_pure_virtual(void)
{
    ASSERT(false);
}

void __cxa_atexit(void)
{
}
