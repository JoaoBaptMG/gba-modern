//--------------------------------------------------------------------------------
// flags-enum.hpp
//--------------------------------------------------------------------------------
// Provides a macro to enable manipulating flags within an enum class
//--------------------------------------------------------------------------------
#pragma once

#include <type_traits>

#define FLAGS_ENUM(T) \
inline static constexpr T operator|(T a, T b) \
{\
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b));\
}\
inline static constexpr T operator&(T a, T b) \
{\
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b));\
}\
inline static constexpr T operator^(T a, T b) \
{\
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^ static_cast<std::underlying_type_t<T>>(b));\
}\
inline static constexpr T operator~(T x) \
{\
    return static_cast<T>(~static_cast<std::underlying_type_t<T>>(x));\
}\
inline static constexpr T& operator|=(T& a, T b) \
{\
    a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b));\
    return a;\
}\
inline static constexpr T& operator&=(T& a, T b) \
{\
    a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b));\
    return a;\
}\
inline static constexpr T& operator^=(T& a, T b) \
{\
    a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^ static_cast<std::underlying_type_t<T>>(b));\
    return a;\
}
