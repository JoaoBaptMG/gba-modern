//--------------------------------------------------------------------------------
// dynamic.cpp
//--------------------------------------------------------------------------------
// Provides overload "new" and "delete" methods to prevent malloc and friends
// from appearing in the final executable
//--------------------------------------------------------------------------------
#include <cstdint>
#include "gba-assert.hpp"

// This function is not defined on purpose
void* operator_new_should_not_be_used(std::size_t s);

void* operator new(std::size_t s) noexcept { return nullptr; }
void* operator new[](std::size_t s) noexcept { return nullptr; }

void operator delete(void*) noexcept { ASSERT(false); }
void operator delete[](void*) noexcept { ASSERT(false); }
void operator delete(void*, std::size_t) noexcept { ASSERT(false); }
void operator delete[](void*, std::size_t) noexcept { ASSERT(false); }
