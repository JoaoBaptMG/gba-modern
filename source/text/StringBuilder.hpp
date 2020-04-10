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

template <std::size_t N>
class InplaceStringBuilder
{
    std::size_t cur;
    char* buffer;

public:
    InplaceStringBuilder(char* buffer) : cur(0), buffer(buffer) { buffer[0] = 0; }

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

    template <std::size_t M>
    void append(const char str[M])
    {
        std::size_t tsize = std::min(N-cur, M-1);
        std::size_t words = tsize/sizeof(u32);
        std::size_t bytes = words*sizeof(u32);

        if (words > 0) memcpy32(buffer+cur, str, words);
        tsize -= bytes;
        cur += bytes;

        while (tsize--) buffer[cur++] = str[bytes++];
    }

    template <typename T>
    constexpr std::enable_if_t<std::is_unsigned_v<T>>
    append(T v)
    {
        char tmp[numCharsInTempBuffer<T>()];
        auto tmpv = uintDigits(tmp, v);
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

    template <typename... Ts>
    std::enable_if_t<sizeof...(Ts) != 0>
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
    char inBuffer[N+1];

public:
    StringBuilder() : InplaceStringBuilder<N>(inBuffer) {}
};
