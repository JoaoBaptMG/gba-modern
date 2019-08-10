#pragma once

#include <cstdint>
#include <vector>

class UnionFind final
{
    std::vector<std::size_t> sets, sizes;
    std::size_t uSets;

    void checkBounds(std::size_t i) const;

public:
    UnionFind(std::size_t numSets);
    std::size_t find(std::size_t id);
    std::size_t find(std::size_t id) const;
    bool doUnion(std::size_t i, std::size_t j);
    inline std::size_t uniqueSets() const { return uSets; }
};
