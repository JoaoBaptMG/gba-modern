//--------------------------------------------------------------------------------
// UnorderedIterableList.hpp
//--------------------------------------------------------------------------------
// Provides a dynamic list of objects of type T, with which it can maintain
// a free list and allocate the objects, and is iterable
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include "LightBitset.hpp"
#include "type_traits.hpp"

template <typename T, std::size_t Count>
class alignas(void*) UnorderedIterableList final
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
    LightBitset<Count> usedCells;

public:
    UnorderedIterableList()
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
        usedCells.set(cell - cells);

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
        usedCells.reset(i);
    }

    void remove(T* obj)
    {
        auto cellPtr = reinterpret_cast<Cell*>(obj);
        ASSERT(cellPtr >= cells && cellPtr < cells+Count);
        removeIndex(cellPtr - cells);
    }

    void clear()
    {
        for (std::size_t i = 0; i < Count; i++)
            if (usedCells.test(i)) std::launder(&cells[i].obj)->~T();
        usedCells.resetAll();

        firstFreeCell = cells;
        for (std::size_t i = 0; i < Count-1; i++)
            cells[i].nextFreeCell = cells+(i+1);
        cells[Count-1].nextFreeCell = nullptr;
    }

    UnorderedIterableList(const UnorderedIterableList&) = delete;
    UnorderedIterableList(UnorderedIterableList&&) = delete;
    UnorderedIterableList &operator=(UnorderedIterableList) = delete;

    ~UnorderedIterableList()
    {
        for (std::size_t i = 0; i < Count; i++)
            if (usedCells.test(i))
                std::launder(&cells[i].obj)->~T();
    }

private:
    // Implementation trick based on https://stackoverflow.com/a/41306206/
    // It is a forward iterator
    template <bool Const>
    class IteratorDetail final
    {
        // A pointer, because we would like to be able to order it
        UnorderedIterableList *list;
        std::size_t id;

    public:
        // Iterator traits
        using difference_type = std::intmax_t;
        using value_type = std::conditional_t<Const, const T&, T&>;
        using pointer = std::remove_reference_t<value_type>*;
        using reference = value_type;
        using iterator_category = std::forward_iterator_tag;

        // The default constructors are enough, and the swap as well
        IteratorDetail() = default;
        IteratorDetail(const IteratorDetail&) = default;
        IteratorDetail(IteratorDetail&&) = default;
        IteratorDetail& operator=(const IteratorDetail&) = default;
        IteratorDetail& operator=(IteratorDetail&&) = default;

        // Iterator constructor
        IteratorDetail(UnorderedIterableList* list, std::size_t id)
            : list(list), id(id) {}

        // Dereference (iterator)
        reference operator*() { return *std::launder(&list->cells[id].obj); }
        const reference operator*() const { return *std::launder(&list->cells[id].obj); }

        // Member dereference (input iterator)
        pointer operator->() { return &operator*(); }
        const pointer operator->() const { return &operator*(); }

        // Check if the iterator is an end iterator
        bool isEnd() const { return list == nullptr || id == Count; }

        // Equality operation (input iterator)
        // All the end iterators compare equal
        bool operator==(const IteratorDetail& o) const
        {
            if (isEnd() || o.isEnd())
                return isEnd() && o.isEnd();
            return list == o.list && id == o.id;
        }

        // Inequality operation (input iterator)
        bool operator!=(const IteratorDetail& o) const { return !(*this == o); }

        // Pre-increment (input iterator)
        IteratorDetail& operator++()
        {
            // Increment until it founds another used cell
            for (id++; id < Count; id++)
                if (list->usedCells.test(id)) break;

            return *this;
        }

        // Post-increment (input iterator)
        IteratorDetail operator++(int)
        {
            IteratorDetail it(*this);
            operator++();
            return it;
        }
    };

    // Begin and end
    template <bool Const>
    IteratorDetail<Const> begin()
    {
        // Find the first used object
        std::size_t id;
        for (id = 0; id < Count; id++)
            if (usedCells.test(id)) break;

        return IteratorDetail<Const>(this, id);
    }

    template <bool Const>
    IteratorDetail<Const> end() { return IteratorDetail<Const>(this, Count); }

public:
    using Iterator = IteratorDetail<false>;
    using ConstIterator = IteratorDetail<true>;

    Iterator begin() { return begin<false>(); }
    ConstIterator begin() const { return begin<true>(); }
    ConstIterator cbegin() const { return begin<true>(); }

    Iterator end() { return end<false>(); }
    ConstIterator end() const { return end<true>(); }
    ConstIterator cend() const { return end<true>(); }
};
