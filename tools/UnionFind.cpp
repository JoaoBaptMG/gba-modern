#include "UnionFind.hpp"

#include <algorithm>
#include <numeric>
#include <stdexcept>

UnionFind::UnionFind(std::size_t numSets)
    : sets(numSets), sizes(numSets, 1), uSets(numSets)
{
    std::iota(sets.begin(), sets.end(), 0);
}

void UnionFind::checkBounds(std::size_t id) const
{
    if (id >= sets.size())
        throw std::out_of_range("Union-find parameter must be inside range!");
}

std::size_t UnionFind::find(std::size_t id)
{
    checkBounds(id);
    if (sets[id] == id) return id;
    return sets[id] = find(sets[id]);
}

std::size_t UnionFind::find(std::size_t id) const
{
    checkBounds(id);
    if (sets[id] == id) return id;
    return find(sets[id]);
}

bool UnionFind::doUnion(std::size_t i, std::size_t j)
{
    checkBounds(i); checkBounds(j);
    i = find(i); j = find(j);
    if (i == j) return false;

    // The number of unique sets reduce every time a set is united
    uSets--;
    if (sizes[i] < sizes[j]) std::swap(i, j);
    sets[j] = i;
    sizes[i] += sizes[j];
    return true;
}
