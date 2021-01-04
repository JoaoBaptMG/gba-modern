//--------------------------------------------------------------------------------
// UnorderedList.hpp
//--------------------------------------------------------------------------------
// Provides a dynamic list of objects of type T, with which it can maintain
// a free list and allocate the objects
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include "LightBitset.hpp"
#include "type_traits.hpp"

template <typename T, std::size_t Count>
class alignas(void*) UnorderedList final
{
    union Cell
    {
        Cell* nextFreeCell;
        alignas(void*) T obj;

        Cell() {}
        ~Cell() {}
    };

    Cell cells[Count];
    Cell* firstFreeCell;

public:
    UnorderedList()
    {
        firstFreeCell = cells;
        for (std::size_t i = 0; i < Count-1; i++)
            cells[i].nextFreeCell = cells+(i+1);
        cells[Count-1].nextFreeCell = nullptr;
    }

    template <typename... Ts>
    T& add(Ts&&... ts)
    {
        ASSERT(firstFreeCell);

        auto cell = firstFreeCell;
        firstFreeCell = cell->nextFreeCell;
        new (&cell->obj) T(std::forward<Ts>(ts)...);

        return cell->obj;
    }

    std::size_t indexOf(T* obj) // figure out a way of removing UB
    {
        return reinterpret_cast<Cell*>(obj) - cells;
    }

    void removeIndex(std::size_t i)
    { 
        std::launder(&cells[i].obj)->~T();
        cells[i].nextFreeCell = firstFreeCell;
        firstFreeCell = cells+i;
    }

    void remove(T* obj)
    {
        auto cellPtr = reinterpret_cast<Cell*>(obj);
        ASSERT(cellPtr >= cells && cellPtr < cells+Count);
        removeIndex(cellPtr - cells);
    }

    void clear()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            // Build a list of not used cells
            LightBitset<Count> freeCells;
            for (auto cell = firstFreeCell; cell; cell = cell->nextFreeCell)
                freeCells.set(cell - cells);

            // And then go through the remaining of the cells
            for (std::size_t i = 0; i < Count; i++)
                if (!freeCells.test(i))
                    std::launder(&cells[i].obj)->~T();
        }

        firstFreeCell = cells;
        for (std::size_t i = 0; i < Count-1; i++)
            cells[i].nextFreeCell = cells+(i+1);
        cells[Count-1].nextFreeCell = nullptr;
    }

    UnorderedList(const UnorderedList&) = delete;
    UnorderedList(UnorderedList&&) = delete;
    UnorderedList &operator=(UnorderedList) = delete;

    ~UnorderedList()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            // Build a list of not used cells
            LightBitset<Count> freeCells;
            for (auto cell = firstFreeCell; cell; cell = cell->nextFreeCell)
                freeCells.set(cell - cells);

            // And then go through the remaining of the cells
            for (std::size_t i = 0; i < Count; i++)
                if (!freeCells.test(i))
                    std::launder(&cells[i].obj)->~T();
        }
    }
};
