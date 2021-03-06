//--------------------------------------------------------------------------------
// UnorderedPolymorphicList.hpp
//--------------------------------------------------------------------------------
// Provides a list of polymorphic objects of base type Base, with which it can
// maintain a free list and allocate the objects, without guaranteeing their
// type, however
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include "LightBitset.hpp"
#include "type_traits.hpp"

template <typename Base, std::size_t CellSize, std::size_t Count>
class alignas(max_align_t) UnorderedPolymorphicList final
{
    static_assert(std::has_virtual_destructor<Base>::value,
                  "UnorderedPolymorphicList will not work properly without virtual destructors!");
    static_assert(sizeof(Base) >= sizeof(std::intptr_t),
                  "UnorderedPolymorphicList must not be smaller than a pointer");

    union Cell
    {
        alignas(void*) std::byte storage[CellSize];
        Cell* nextFreeCell;
    };

    Cell cells[Count];
    Cell* firstFreeCell;
    LightBitset<Count> usedCells;

public:
    UnorderedPolymorphicList()
    {
        firstFreeCell = cells;
        for (std::size_t i = 0; i < Count-1; i++)
            cells[i].nextFreeCell = cells+(i+1);
        cells[Count-1].nextFreeCell = nullptr;
    }

    template <typename Derived, typename... Ts>
    void add(Ts&&... ts)
    {
        static_assert(std::is_base_of_v<Base, Derived>,
            "The Derived class must be derived from Base!");
        static_assert(sizeof(Derived) <= CellSize,
            "The Derived class is too big to fit in that UnorderedPolymorphicList");
        static_assert(!is_virtual_base_of_v<Base, Derived>,
            "UnorderedPolymorphicList does not work properly with virtual base classes!");

        ASSERT(firstFreeCell);
        if (firstFreeCell)
        {
            auto cell = firstFreeCell;
            firstFreeCell = cell->nextFreeCell;
            new (cell->storage) Derived(std::forward<Ts>(ts)...);
            usedCells.set(cell - cells);
        }
    }

    std::size_t indexOf(Base* obj) // TODO: figure out a way of removing UB
    {
        return reinterpret_cast<Cell*>(obj) - cells;
    }

    void removeIndex(std::size_t i)
    { 
        std::launder(reinterpret_cast<Base*>(cells[i].storage))->~Base();
        cells[i].nextFreeCell = firstFreeCell;
        firstFreeCell = cells+i;
        usedCells.reset(i);
    }

    void remove(Base* obj)
    {
        auto cellPtr = reinterpret_cast<Cell*>(obj);
        ASSERT(cellPtr >= cells && cellPtr < cells+Count);
        removeCell(cellPtr - cells);
    }

    UnorderedPolymorphicList(const UnorderedPolymorphicList&) = delete;
    UnorderedPolymorphicList(UnorderedPolymorphicList&&) = delete;
    UnorderedPolymorphicList &operator=(UnorderedPolymorphicList) = delete;

    ~UnorderedPolymorphicList()
    {
        for (std::size_t i = 0; i < Count; i++)
            if (usedCells.test(i))
                std::launder(reinterpret_cast<Base*>(cells[i].storage))->~Base();
    }

private:
    // Implementation trick based on https://stackoverflow.com/a/41306206/
    // It is a forward iterator
    template <bool Const>
    class IteratorDetail final
    {
        // A pointer, because we would like to be able to order it
        UnorderedPolymorphicList *list;
        std::size_t id;

    public:
        // Iterator traits
        using difference_type = std::intmax_t;
        using value_type = std::conditional_t<Const, const Base&, Base&>;
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
        IteratorDetail(UnorderedPolymorphicList* list, std::size_t id)
            : list(list), id(id) {}

        // Dereference (iterator)
        reference operator*() { return *std::launder(reinterpret_cast<pointer>(list->cells[id].storage)); }
        const reference operator*() const { return *std::launder(reinterpret_cast<const pointer>(list->cells[id].storage)); }

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
