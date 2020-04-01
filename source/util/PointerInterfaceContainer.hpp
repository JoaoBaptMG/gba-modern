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
#include <utility>
#include "type_traits.hpp"

template <typename Base, std::size_t Size>
class PointerInterfaceContainer
{
    static_assert(std::is_default_constructible_v<Base>,
        "PointerInterfaceContainer will not work without a Base that is default constructible!");
    static_assert(std::has_virtual_destructor_v<Base>,
        "PointerInterfaceContainer will not work properly without virtual destructors!");
    static_assert(sizeof(Base) >= sizeof(std::intptr_t),
        "PointerInterfaceContainer must not be smaller than a pointer");

    alignas(void*) std::byte storage[Size];

public:
    PointerInterfaceContainer() { new (storage) Base(); }

    template <typename Derived, typename... Ts>
    void assign(Ts&&... ts)
    {
        static_assert(std::is_base_of_v<Base, Derived>,
            "The Derived class must be derived from Base!");
        static_assert(sizeof(Derived) <= Size,
            "The Derived class is too big to fit in that PointerInterfaceContainer");
        static_assert(!is_virtual_base_of_v<Base, Derived>,
            "PointerInterfaceContainer does not work properly with virtual base classes!");

        std::launder(reinterpret_cast<Base*>(storage))->~Base();
        new (storage) Derived(std::forward<Ts>(ts)...);
    }

    void clear() { assign<Base>(); }

    PointerInterfaceContainer(const PointerInterfaceContainer&) = delete;
    PointerInterfaceContainer(PointerInterfaceContainer&&) = delete;
    PointerInterfaceContainer &operator=(PointerInterfaceContainer) = delete;

    Base* operator->() { return std::launder(reinterpret_cast<Base*>(storage)); }
    const Base* operator->() const { return std::launder(reinterpret_cast<const Base*>(storage)); }

    Base& operator*() { return *std::launder(reinterpret_cast<Base*>(storage)); }
    const Base& operator*() const { return *std::launder(reinterpret_cast<const Base*>(storage)); }

    ~PointerInterfaceContainer()
    {
        std::launder(reinterpret_cast<Base*>(storage))->~Base();
    }
};
