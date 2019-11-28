//--------------------------------------------------------------------------------
// integer-sequence-utils.hpp
//--------------------------------------------------------------------------------
// Provides some functions to help managing integer sequences
//--------------------------------------------------------------------------------
#pragma once

#include <utility>

// Calculates 2^x for each x in the sequence
template <typename T> struct exp2_seq;
template <typename T, T... Nums>
struct exp2_seq<std::integer_sequence<T, Nums...>>
{
    using type = std::integer_sequence<T, (1 << Nums)...>;
};
template <typename T> using exp2_seq_t = typename exp2_seq<T>::type;

// Calculates the sum of all elements in the sequence
template <typename T> struct seq_sum;
template <typename T, T... Nums>
struct seq_sum<std::integer_sequence<T, Nums...>>
    : std::integral_constant<T, (T(0) + ... + Nums)> {};
template <typename T>
constexpr auto seq_sum_v = seq_sum<T>::value;

// Calculates the exclusive sum of an integer sequence, meaning:
// returns U from T where U[x] = T[0] + T[1] + ... T[N-1]
namespace detail
{
    template <typename T, typename Cur, T, typename>
    struct prefix_sum_helper { using type = Cur; };

    template <typename T, T... CurSeq, T Next, T Head, T... Tail>
    struct prefix_sum_helper<T, std::integer_sequence<T, CurSeq...>, Next,
        std::integer_sequence<T, Head, Tail...>>
        : prefix_sum_helper<T, std::integer_sequence<T, CurSeq..., Next>,
            Next+Head, std::integer_sequence<T, Tail...>> {};
}

template <typename T>
struct prefix_sum;

template <typename T, T... Nums>
struct prefix_sum<std::integer_sequence<T, Nums...>>
    : detail::prefix_sum_helper<T, std::integer_sequence<T>,
        T(0), std::integer_sequence<T, Nums...>> {};

template <typename T> using prefix_sum_t = typename prefix_sum<T>::type;
