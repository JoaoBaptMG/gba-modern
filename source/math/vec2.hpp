//--------------------------------------------------------------------------------
// vec2.hpp
//--------------------------------------------------------------------------------
// Provides a generic vector math library
//--------------------------------------------------------------------------------
#pragma once

#include "util/type_traits.hpp"

template <typename T>
struct vec2 final
{
    static_assert(std::is_integral_v<T>, "vec2 only works properly with integral types (including fixed)!");

    T x, y;

    // Default constructors
    constexpr vec2() = default;
    constexpr vec2(const vec2&) = default;
    constexpr vec2(vec2&&) = default;
    constexpr vec2& operator=(const vec2&) = default;
    constexpr vec2& operator=(vec2&&) = default;

    // Normal constructor
    constexpr vec2(T x, T y) : x(x), y(y) {}

    // Assignment operators
    template <typename U>
    constexpr vec2& operator+=(vec2<auto> o) { x += o.x; y += o.y; return *this; }
    template <typename U>
    constexpr vec2& operator-=(vec2<U> o) { x -= o.x; y -= o.y; return *this; }
    template <typename U>
    constexpr vec2& operator&=(vec2<U> o) { x &= o.x; y &= o.y; return *this; }
    template <typename U>
    constexpr vec2& operator|=(vec2<U> o) { x |= o.x; y |= o.y; return *this; }
    template <typename U>
    constexpr vec2& operator^=(vec2<U> o) { x ^= o.x; y ^= o.y; return *this; }

    template <typename U>
    constexpr vec2& operator*=(U v) { x *= v; y *= v; return *this; }
    template <typename U>
    constexpr vec2& operator/=(U v) { x /= v; y /= v; return *this; }
    template <typename U>
    constexpr vec2& operator%=(U v) { x %= v; y %= v; return *this; }

    constexpr vec2& operator<<=(int n) { x <<= n; y <<= n; return *this; }
    constexpr vec2& operator>>=(int n) { x >>= n; y >>= n; return *this; }

    // Unary operators
    constexpr vec2 operator~() const { return vec2(~x, ~y); }
    constexpr vec2 operator-() const { return vec2(-x, -y); }

    // Conversion operators
    template <typename U, typename = std::enable_if_t<!std::is_same_v<T, U> && std::is_convertible_v<T, U>>, typename = void>
    constexpr operator vec2<U>() const { return vec2<U>(x, y); }
    template <typename U, typename = std::enable_if_t<!std::is_same_v<T, U> && is_explicitly_convertible_v<T, U>>>
    constexpr explicit operator vec2<U>() const { return vec2<U>(U(x), U(y)); }

    // Utilities
    constexpr auto dot(vec2 o) const { return x*o.x + y*o.y; }
    constexpr auto cross(vec2 o) const { return x*o.y - y*o.x; }
};

// Template deduction guide
template <typename T> vec2(T x, T y) -> vec2<T>;

// Free function operators
template <typename T, typename U>
constexpr auto operator+(vec2<T> a, vec2<U> b) { return vec2(a.x + b.x, a.y + b.y); }
template <typename T, typename U>
constexpr auto operator-(vec2<T> a, vec2<U> b) { return vec2(a.x - b.x, a.y - b.y); }
template <typename T, typename U>
constexpr auto operator&(vec2<T> a, vec2<U> b) { return vec2(a.x & b.x, a.y & b.y); }
template <typename T, typename U>
constexpr auto operator|(vec2<T> a, vec2<U> b) { return vec2(a.x | b.x, a.y | b.y); }
template <typename T, typename U>
constexpr auto operator^(vec2<T> a, vec2<U> b) { return vec2(a.x ^ b.x, a.y ^ b.y); }

template <typename T, typename U>
constexpr auto operator*(vec2<T> v, U s) { return vec2(v.x * s, v.y * s); }
template <typename T, typename U>
constexpr auto operator*(T s, vec2<U> v) { return vec2(s * v.x, s * v.y); }
template <typename T, typename U>
constexpr auto operator/(vec2<T> v, U s) { return vec2(v.x / s, v.y / s); }
template <typename T, typename U>
constexpr auto operator%(vec2<T> v, U s) { return vec2(v.x % s, v.y % s); }

template <typename T>
constexpr auto operator<<(vec2<T> v, int n) { return vec2(v.x << n, v.y << n); }
template <typename T>
constexpr auto operator>>(vec2<T> v, int n) { return vec2(v.x >> n, v.y >> n); }
