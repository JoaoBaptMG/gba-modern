//--------------------------------------------------------------------------------
// PointerInterfaceContainer.hpp
//--------------------------------------------------------------------------------
// Provides a class that can effectively allocate objects derived from a
// base class and expose them as pointers from that base
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include "type_traits.hpp"

template <typename Base, std::size_t Size>
class alignas(max_align_t) PointerInterfaceContainer
{
    static_assert(std::has_virtual_destructor<Base>::value,
        "PointerInterfaceContainer will not work properly without virtual destructors!");
    static_assert(sizeof(Base) >= sizeof(std::intptr_t),
        "PointerInterfaceContainer must not be smaller than a pointer");

    std::byte storage[Size];

public:
    PointerInterfaceContainer()
    {
        *reinterpret_cast<std::intptr_t*>(storage) = 0;
    }

    template <typename Derived, typename... Ts>
    std::enable_if_t<std::is_base_of_v<Base, Derived>>
    assign(Ts&&... ts)
    {
        static_assert(sizeof(Derived) <= Size,
            "The Derived class is too big to fit in that PointerInterfaceContainer");
        static_assert(!is_virtual_base_of_v<Base, Derived>,
            "PointerInterfaceContainer does not work properly with virtual base classes!");

        if (!empty()) reinterpret_cast<Base*>(storage)->~Base();
        new (storage) Derived(std::forward<Ts>(ts)...);
    }

    void clear()
    { 
        if (!empty()) reinterpret_cast<Base*>(storage)->~Base();
        *reinterpret_cast<std::intptr_t*>(storage) = 0;
    }

    PointerInterfaceContainer(const PointerInterfaceContainer&) = delete;
    PointerInterfaceContainer(PointerInterfaceContainer&&) = delete;
    PointerInterfaceContainer &operator=(PointerInterfaceContainer) = delete;

    Base* operator->() { return reinterpret_cast<Base*>(storage); }
    const Base* operator->() const { return reinterpret_cast<const Base*>(storage); }

    Base& operator*() { return *reinterpret_cast<Base*>(storage); }
    const Base& operator*() const { return *reinterpret_cast<const Base*>(storage); }

    bool empty() const // This function might cause undefined behavior
    { 
        return *reinterpret_cast<const std::intptr_t*>(storage) == 0;
    }

    explicit operator bool() const { return !empty(); }

    ~PointerInterfaceContainer()
    {
        if (!empty()) reinterpret_cast<Base*>(storage)->~Base();
    }
};
