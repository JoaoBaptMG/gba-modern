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
#include <array>

template <std::size_t N>
class StringBuilder final
{
    std::size_t cur;
    std::array<char, N+1> buffer;

public:
    constexpr StringBuilder() : cur(0), buffer{0} {}

    constexpr void append(char ch)
    {
        if (cur < N) buffer[cur++] = ch;
    }

    constexpr void append(const char* str)
    {
        for (auto p = str; *p; p++)
        {
            if (cur == N) break;
            buffer[cur++] = *p;
        }
    }

    template <std::size_t M>
    constexpr void append(const char str[M])
    {
        std::size_t tsize = std::min(N-cur, M-1);
        std::copy(str, str+tsize, buffer+cur);
        cur += tsize;
    }

    template <typename T>
    constexpr std::enable_if_t<std::is_unsigned_v<T>>
    append(T v)
    {
        char tmp[std::numeric_limits<T>::digits10] = {0};
        std::size_t i = 0;
        T oldv = v;

        do
        {
            v /= T(10);
            tmp[i++] = oldv - v * T(10);
            oldv = v;
        } while (v);

        while (i-- && cur < N) buffer[cur++] = '0' + tmp[i];
    }

    template <typename T>
    constexpr std::enable_if_t<std::is_signed_v<T>>
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
    constexpr std::enable_if_t<sizeof...(Ts) != 0>
    append(Ts&&... vs)
    {
        ((void)append(std::forward<Ts>(vs)), ...);
    }

    constexpr auto numCharactersWritten() const { return cur; }

    constexpr std::array<char, N+1> getString()
    { 
        buffer[cur] = 0;
        return buffer;
    }
};