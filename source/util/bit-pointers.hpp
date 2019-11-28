//--------------------------------------------------------------------------------
// bit-pointers.hpp
//--------------------------------------------------------------------------------
// Provides functions to enable bit-arithmetic operation in pointers
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

template <typename T>
void setBits(T*& ptr, std::uintptr_t val)
{
    ptr = reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(ptr) | val);
}

template <typename T>
void resetBits(T *&ptr, std::uintptr_t val)
{
    ptr = reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(ptr) & ~val);
}

template <typename T>
bool hasOneOf(T *ptr, std::uintptr_t val)
{
    return reinterpret_cast<std::uintptr_t>(ptr) & val;
}