//--------------------------------------------------------------------------------
// generateTable.hpp
//--------------------------------------------------------------------------------
// A simple contexpr function to generate lookup tables
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <array>

template <std::size_t N, typename Generator>
constexpr auto generateTable(Generator g)
{
    using Ret = decltype(std::declval<Generator>()(std::declval<std::size_t>()));
    std::array<Ret, N> table{};

    for (std::size_t i = 0; i < N; i++)
        table[i] = g(i);

    return table;
}
