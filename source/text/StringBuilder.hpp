//--------------------------------------------------------------------------------
// StringBuilder.hpp
//--------------------------------------------------------------------------------
// Class that provides a way to build strings by appending characters, other
// strings and numeric formats
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <tonc.h>
#include "util/uintDigits.h"

template <typename T>
constexpr std::size_t numCharsInTempBuffer()
{
    auto num = std::numeric_limits<T>::digits10;
    return (num + 3) & ~3;
}

template <typename T>
struct HexMarker { T v; };

template <typename T>
inline static std::enable_if_t<std::is_unsigned_v<T>, HexMarker<T>>
hex(T v) { return {v}; }

template <std::size_t N>
class InplaceStringBuilder
{
    std::size_t cur;
    char* buffer;

public:
    InplaceStringBuilder(char* buffer) : cur(0), buffer(buffer) { buffer[0] = 0; }

    // Disable copying and moving
    InplaceStringBuilder(const InplaceStringBuilder&) = delete;
    InplaceStringBuilder(InplaceStringBuilder&&) = delete;
    InplaceStringBuilder& operator=(InplaceStringBuilder) = delete;

    void append(char ch)
    {
        if (cur < N) buffer[cur++] = ch;
    }

    void append(const char* str)
    {
        for (auto p = str; *p; p++)
        {
            if (cur == N) break;
            buffer[cur++] = *p;
        }
    }

    template <typename T>
    std::enable_if_t<std::is_unsigned_v<T>>
    append(T v)
    {
        char tmp[numCharsInTempBuffer<T>()];
        auto tmpv = uintDigits(tmp, v);
        while (tmpv != tmp && cur < N) buffer[cur++] = '0' + *--tmpv;
    }

    template <typename T>
    std::enable_if_t<std::is_unsigned_v<T>>
    append(HexMarker<T> h)
    {
        char tmp[2 * sizeof(T)];
        
        auto tmpv = tmp;
        do
        {
            *tmpv = h.v & 0xF;
            if (*tmpv > 9) *tmpv += 'A' - ('9' + 1);
            tmpv++;
            h.v >>= 4;
        } while (h.v);

        while (tmpv != tmp && cur < N) buffer[cur++] = '0' + *--tmpv;
    }

    template <typename T>
    std::enable_if_t<std::is_signed_v<T>>
    append(T v)
    {
        if (v < T(0))
        {
            if (cur < N) buffer[cur++] = '-';
            v = -v;
        }
        append(std::make_unsigned_t<T>(v));
    }

    template <std::size_t M>
    void append(const InplaceStringBuilder<M>& sb2)
    {
        for (std::size_t i = 0; i < sb2.cur && cur < N; i++, cur++)
            buffer[cur] = sb2.buffer[i];
    }

    template <typename... Ts>
    std::enable_if_t<(sizeof...(Ts) > 1)>
    append(Ts&&... vs)
    {
        (append(std::forward<Ts>(vs)), ...);
    }

    const char* getString()
    { 
        buffer[cur] = 0;
        return buffer;
    }
};

template <std::size_t N>
class StringBuilder final : public InplaceStringBuilder<N>
{
    alignas(void*) char inBuffer[N+1];

public:
    StringBuilder() : InplaceStringBuilder<N>(inBuffer) {}
};
