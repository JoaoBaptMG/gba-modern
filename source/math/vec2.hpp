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
    vec2() = default;
    vec2(const vec2&) = default;
    vec2(vec2&&) = default;
    vec2& operator=(const vec2&) = default;
    vec2& operator=(vec2&&) = default;

    // Normal constructor
    vec2(T x, T y) : x(x), y(y) {}

    // Assignment operators
    vec2& operator+=(vec2 o) { x += o.x; y += o.y; return *this; }
    vec2& operator-=(vec2 o) { x -= o.x; y -= o.y; return *this; }
    vec2& operator&=(vec2 o) { x &= o.x; y &= o.y; return *this; }
    vec2& operator|=(vec2 o) { x |= o.x; y |= o.y; return *this; }
    vec2& operator^=(vec2 o) { x ^= o.x; y ^= o.y; return *this; }

    vec2& operator*=(T v) { x *= v; y *= v; return *this; }
    vec2& operator/=(T v) { x /= v; y /= v; return *this; }
    vec2& operator%=(T v) { x %= v; y %= v; return *this; }

    vec2& operator<<=(int n) { x <<= n; y <<= n; return *this; }
    vec2& operator>>=(int n) { x >>= n; y >>= n; return *this; }

    // Unary operators
    vec2 operator~() const { return vec2(~x, ~y); }
    vec2 operator-() const { return vec2(-x, -y); }

    // Conversion operators
    template <typename U, typename = std::enable_if_t<!std::is_same_v<T, U> && std::is_convertible_v<T, U>>, typename = void>
    operator vec2<U>() const { return vec2<U>(x, y); }
    template <typename U, typename = std::enable_if_t<!std::is_same_v<T, U> && is_explicitly_convertible_v<T, U>>>
    explicit operator vec2<U>() const { return vec2<U>(U(x), U(y)); }

    // Utilities
    auto dot(vec2 o) const { return x*o.x + y*o.y; }
    auto cross(vec2 o) const { return x*o.y - y*o.x; }
};

// Template deduction guide
template <typename Ty> vec2(Ty x, Ty y) -> vec2<Ty>;

// Free function operators
template <typename T>
auto operator+(vec2<T> a, vec2<T> b) { return vec2(a.x + b.x, a.y + b.y); }
template <typename T>
auto operator-(vec2<T> a, vec2<T> b) { return vec2(a.x - b.x, a.y - b.y); }
template <typename T>
auto operator&(vec2<T> a, vec2<T> b) { return vec2(a.x & b.x, a.y & b.y); }
template <typename T>
auto operator|(vec2<T> a, vec2<T> b) { return vec2(a.x | b.x, a.y | b.y); }
template <typename T>
auto operator^(vec2<T> a, vec2<T> b) { return vec2(a.x ^ b.x, a.y ^ b.y); }

template <typename T>
auto operator*(vec2<T> v, T s) { return vec2(v.x * s, v.y * s); }
template <typename T>
auto operator*(T s, vec2<T> v) { return vec2(s * v.x, s * v.y); }
template <typename T>
auto operator/(vec2<T> v, T s) { return vec2(v.x / s, v.y / s); }
template <typename T>
auto operator%(vec2<T> v, T s) { return vec2(v.x % s, v.y % s); }

template <typename T>
auto operator<<(vec2<T> v, int n) { return vec2(v.x << n, v.y << n); }
template <typename T>
auto operator>>(vec2<T> v, int n) { return vec2(v.x >> n, v.y >> n); }
