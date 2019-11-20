//--------------------------------------------------------------------------------
// flags-enum.hpp
//--------------------------------------------------------------------------------
// Provides a macro to enable manipulating flags within an enum class
//--------------------------------------------------------------------------------
#pragma once

#include <type_traits>

#define FLAGS_ENUM(T)                                                   \
    inline static constexpr T operator|(T a, T b)                       \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        return static_cast<T>(static_cast<UT>(a) | static_cast<UT>(b)); \
    }                                                                   \
    inline static constexpr T operator&(T a, T b)                       \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        return static_cast<T>(static_cast<UT>(a) & static_cast<UT>(b)); \
    }                                                                   \
    inline static constexpr T operator^(T a, T b)                       \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        return static_cast<T>(static_cast<UT>(a) ^ static_cast<UT>(b)); \
    }                                                                   \
    inline static constexpr T operator~(T x)                            \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        return static_cast<T>(~static_cast<UT>(x));                     \
    }                                                                   \
    inline static constexpr T& operator|=(T& a, T b)                    \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        a = static_cast<T>(static_cast<UT>(a) | static_cast<UT>(b));    \
        return a;                                                       \
    }                                                                   \
    inline static constexpr T& operator&=(T& a, T b)                    \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        a = static_cast<T>(static_cast<UT>(a) & static_cast<UT>(b));    \
        return a;                                                       \
    }                                                                   \
    inline static constexpr T& operator^=(T& a, T b)                    \
    {                                                                   \
        using UT = std::underlying_type_t<T>;                           \
        a = static_cast<T>(static_cast<UT>(a) ^ static_cast<UT>(b));    \
        return a;                                                       \
    }
