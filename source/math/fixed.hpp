//--------------------------------------------------------------------------------
// fixed.hpp
//--------------------------------------------------------------------------------
// Provides a generic fixed-point math library
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>

constexpr std::size_t max(std::size_t a, std::size_t b)
{
    return a > b ? a : b;
}

struct direct_t {};
constexpr direct_t direct = {};

template <typename Ty, std::size_t N>
class fixed final
{
    static_assert(std::is_integral_v<Ty>,
        "Only integral types can be used with fixed!");

    Ty value;

public:
    template <typename T2, std::size_t M>
    friend class fixed;

    static constexpr auto epsilon() noexcept { return fixed(direct, 1); }

    constexpr fixed() = default;
    constexpr fixed(const Ty& v) : value(v << N) {}
    constexpr fixed(direct_t, const Ty& v) : value(v) {}

    // This is to make the compiler prefer the other code with integral 
    template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>, void>>
    constexpr fixed(T v) : value(v * (1 << N)) {}

    template <typename Ty2, std::size_t M>
    constexpr fixed(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value = o.value;
        if constexpr (N > M) value = o.value << (N - M);
        if constexpr (N < M) value = o.value >> (M - N);
    }

    constexpr std::size_t exp() const { return N; }
    constexpr Ty raw() const { return value; }

    template<typename Ty2>
    constexpr fixed<Ty2, N> with_type() const { return fixed<Ty2, N>(*this); }

    template<std::size_t M>
    constexpr fixed<Ty, M> with_exp() const { return fixed<Ty, M>(*this); }

    template <typename U, typename = std::enable_if_t<std::is_integral_v<U>>>
    constexpr operator U() const { return value >> N; }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator=(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value = o.value;
        if constexpr (N > M) value = o.value << (N - M);
        if constexpr (N < M) value = o.value >> (M - N);
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator=(U v) { value = v << N; return *this; }

    template <typename U>
    constexpr std::enable_if_t<std::is_floating_point_v<U>, fixed&>
    operator=(U v) { value = v * (1 << N); return *this; }

    // volatile overload needed so the class can be used with hardware registers
    template <typename Ty2, std::size_t M>
    constexpr void operator=(fixed<Ty2, M> o) volatile
    {
        if constexpr (N == M) value = o.value;
        if constexpr (N > M) value = o.value << (N - M);
        if constexpr (N < M) value = o.value >> (M - N);
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, volatile fixed&>
    operator=(U v) volatile { value = v << N; return *this; }

    template <typename U>
    constexpr std::enable_if_t<std::is_floating_point_v<U>, volatile fixed&>
    operator=(U v) volatile { value = v * (1 << N); return *this; }

    // Compound assignment operators
    template <typename Ty2, std::size_t M>
    constexpr fixed& operator+=(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value += o.value;
        if constexpr (N > M) value += o.value << (N - M);
        if constexpr (N < M) value += o.value >> (M - N);
        return *this;
    }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator-=(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value -= o.value;
        if constexpr (N > M) value -= o.value << (N - M);
        if constexpr (N < M) value -= o.value >> (M - N);
        return *this;
    }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator&=(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value &= o.value;
        if constexpr (N > M) value &= o.value << (N - M);
        if constexpr (N < M) value &= o.value >> (M - N);
        return *this;
    }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator|=(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value |= o.value;
        if constexpr (N > M) value |= o.value << (N - M);
        if constexpr (N < M) value |= o.value >> (M - N);
        return *this;
    }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator^=(fixed<Ty2, M> o)
    {
        if constexpr (N == M) value ^= o.value;
        if constexpr (N > M) value ^= o.value << (N - M);
        if constexpr (N < M) value ^= o.value >> (M - N);
        return *this;
    }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator*=(fixed<Ty2, M> o)
    {
        value = (value * o.value) >> M;
        return *this;
    }

    template <typename Ty2, std::size_t M>
    constexpr fixed& operator/=(fixed<Ty2, M> o)
    {
        value = (value >> M) / o.value;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator+=(U v)
    {
        value += v << N;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator-=(U v)
    {
        value -= v << N;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator&=(U v)
    {
        value &= v << N;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator|=(U v)
    {
        value |= v << N;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator^=(U v)
    {
        value ^= v << N;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator*=(U v)
    {
        value *= v;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator/=(U v)
    {
        value /= v;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator<<=(U v)
    {
        value <<= v;
        return *this;
    }

    template <typename U>
    constexpr std::enable_if_t<std::is_integral_v<U>, fixed&>
    operator>>=(U v)
    {
        value >>= v;
        return *this;
    }
};

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator+(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    constexpr auto Y = max(N, M);

    if constexpr (N == M)
    {
        auto v = a.raw() + b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N > M)
    {
        auto v = a.raw() + (b.raw() << (N - M));
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N < M)
    {
        auto v = (a.raw() << (M - N)) + b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator-(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    constexpr auto Y = max(N, M);

    if constexpr (N == M)
    {
        auto v = a.raw() - b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N > M)
    {
        auto v = a.raw() - (b.raw() << (N - M));
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N < M)
    {
        auto v = (a.raw() << (M - N)) - b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator*(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    auto v = a.raw() * b.raw();
    return fixed<decltype(v), N+M>(direct, v);
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator/(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    auto v = a.raw() / b.raw();
    return fixed<decltype(v), N-M>(direct, v);
}

template <typename Ty, std::size_t N>
constexpr auto operator-(fixed<Ty, N> k)
{
    return fixed<Ty, N>(direct, -k.raw());
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator&(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    constexpr auto Y = max(N, M);

    if constexpr (N == M)
    {
        auto v = a.raw() & b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N > M)
    {
        auto v = a.raw() & (b.raw() << (N - M));
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N < M)
    {
        auto v = (a.raw() << (M - N)) & b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator|(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    constexpr auto Y = max(N, M);

    if constexpr (N == M)
    {
        auto v = a.raw() | b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N > M)
    {
        auto v = a.raw() | (b.raw() << (N - M));
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N < M)
    {
        auto v = (a.raw() << (M - N)) | b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator^(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    constexpr auto Y = max(N, M);

    if constexpr (N == M)
    {
        auto v = a.raw() ^ b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N > M)
    {
        auto v = a.raw() ^ (b.raw() << (N - M));
        return fixed<decltype(v), Y>(direct, v);
    }
    if constexpr (N < M)
    {
        auto v = (a.raw() << (M - N)) ^ b.raw();
        return fixed<decltype(v), Y>(direct, v);
    }
}

template <typename Ty, std::size_t N>
constexpr auto operator~(fixed<Ty, N> k)
{
    return fixed<Ty, N>(direct, ~k.raw());
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator==(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    if constexpr (N == M) return a.raw() == b.raw();
    if constexpr (N > M) return a.raw() == (b.raw() << (N - M));
    if constexpr (N < M) return (a.raw() << (M - N)) == b.raw();
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator!=(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    if constexpr (N == M) return a.raw() != b.raw();
    if constexpr (N > M) return a.raw() != (b.raw() << (N - M));
    if constexpr (N < M) return (a.raw() << (M - N)) != b.raw();
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator<(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    if constexpr (N == M) return a.raw() < b.raw();
    if constexpr (N > M) return a.raw() < (b.raw() << (N - M));
    if constexpr (N < M) return (a.raw() << (M - N)) < b.raw();
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator>(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    if constexpr (N == M) return a.raw() > b.raw();
    if constexpr (N > M) return a.raw() > (b.raw() << (N - M));
    if constexpr (N < M) return (a.raw() << (M - N)) > b.raw();
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator<=(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    if constexpr (N == M) return a.raw() <= b.raw();
    if constexpr (N > M) return a.raw() <= (b.raw() << (N - M));
    if constexpr (N < M) return (a.raw() << (M - N)) <= b.raw();
}

template <typename Ty1, std::size_t N, typename Ty2, std::size_t M>
constexpr auto operator>=(fixed<Ty1, N> a, fixed<Ty2, M> b)
{
    if constexpr (N == M) return a.raw() >= b.raw();
    if constexpr (N > M) return a.raw() >= (b.raw() << (N - M));
    if constexpr (N < M) return (a.raw() << (M - N)) >= b.raw();
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator+(fixed<Ty, N> a, U b)
{
    auto v = a.raw() + (b << N);
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator+(U b, fixed<Ty, N> a)
{
    auto v = (b << N) + a.raw();
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator-(fixed<Ty, N> a, U b)
{
    auto v = a.raw() - (b << N);
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator-(U b, fixed<Ty, N> a)
{
    auto v = (b << N) - a.raw();
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator&(fixed<Ty, N> a, U b)
{
    auto v = a.raw() & (b << N);
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator&(U b, fixed<Ty, N> a)
{
    auto v = (b << N) & a.raw();
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator|(fixed<Ty, N> a, U b)
{
    auto v = a.raw() | (b << N);
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator|(U b, fixed<Ty, N> a)
{
    auto v = (b << N) | a.raw();
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator^(fixed<Ty, N> a, U b)
{
    auto v = a.raw() ^ (b << N);
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator^(U b, fixed<Ty, N> a)
{
    auto v = (b << N) ^ a.raw();
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator*(fixed<Ty, N> a, U b)
{
    auto v = a.raw() * b;
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator*(U b, fixed<Ty, N> a)
{
    auto v = b * a.raw();
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator/(fixed<Ty, N> a, U b)
{
    auto v = a.raw() / b;
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator<<(fixed<Ty, N> a, U b)
{
    auto v = a.raw() << b;
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, fixed<Ty, N>>
operator>>(fixed<Ty, N> a, U b)
{
    auto v = a.raw() >> b;
    return fixed<decltype(v), N>(direct, v);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator==(fixed<Ty, N> a, U b)
{
    return a.raw() == (b << N);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator==(U b, fixed<Ty, N> a)
{
    return (b << N) == a.raw();
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator!=(fixed<Ty, N> a, U b)
{
    return a.raw() != (b << N);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator!=(U b, fixed<Ty, N> a)
{
    return (b << N) != a.raw();
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator<(fixed<Ty, N> a, U b)
{
    return a.raw() < (b << N);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator<(U b, fixed<Ty, N> a)
{
    return (b << N) < a.raw();
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator>(fixed<Ty, N> a, U b)
{
    return a.raw() > (b << N);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator>(U b, fixed<Ty, N> a)
{
    return (b << N) > a.raw();
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator<=(fixed<Ty, N> a, U b)
{
    return a.raw() <= (b << N);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator<=(U b, fixed<Ty, N> a)
{
    return (b << N) <= a.raw();
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator>=(fixed<Ty, N> a, U b)
{
    return a.raw() >= (b << N);
}

template <typename Ty, std::size_t N, typename U>
constexpr std::enable_if_t<std::is_integral_v<U>, bool>
operator>=(U b, fixed<Ty, N> a)
{
    return (b << N) >= a.raw();
}

template <typename Ty, std::size_t N>
constexpr fixed<Ty,N> abs(fixed<Ty,N> v) { return v < 0 ? -v : v; }

template <typename T>
struct is_fixed : std::false_type {};

template <typename Ty, std::size_t N>
struct is_fixed<fixed<Ty, N>> : std::true_type {};

template <typename T>
constexpr bool is_fixed_v = is_fixed<T>::value;
