//--------------------------------------------------------------------------------
// AllocatorPointer.hpp
//--------------------------------------------------------------------------------
// RAII wrapper over any object that implements a simple allocator pattern
//--------------------------------------------------------------------------------

#pragma once

#include <type_traits>
#include "bit-pointers.hpp"

// An allocator class that uses CRTP to access its derived type
template <typename T>
class Allocator
{
    u16 refs;

protected:
    // Increases the reference counter
    void retain()
    {
        if (refs == 0) static_cast<T*>(this)->alloc();
        refs++;
    }

    // Decreases the reference counter
    void release()
    {
        refs--;
        if (refs == 0) static_cast<T*>(this)->clear();
    }

public:
    Allocator() : refs(0) {}
    ~Allocator() { static_cast<T*>(this)->clear(); }

    template <typename U>
    friend class AllocatorPointer;
};

template <typename T>
class AllocatorPointer
{
    static_assert(std::is_base_of_v<Allocator<T>, T>, "The allocator type must be derived from Allocator!");

    // Since the address space of the GBA has only 28 bits, this is safe to do
    constexpr static std::uintptr_t InactiveBit = 1 << 31;

protected:
    T *allocator;

public:
    AllocatorPointer() : allocator(nullptr) {}

    AllocatorPointer(T& allocator, bool active = true) : allocator(&allocator)
    {
        if (active) allocator.retain();
        else setBits(this->allocator, InactiveBit);
    }

    AllocatorPointer(const AllocatorPointer& o) : allocator(o.allocator)
    {
        if (o.active()) allocator->retain();
    }

    AllocatorPointer& operator=(const AllocatorPointer& o)
    {
        if (active()) allocator->release();
        allocator = o.allocator;
        if (active()) allocator->retain();
        return *this;
    }

    AllocatorPointer(AllocatorPointer&& o) : allocator(o.allocator)
    {
        o.allocator = nullptr;
    }

    AllocatorPointer& operator=(AllocatorPointer&& o)
    {
        std::swap(allocator, o.allocator);
        return *this;
    }

    ~AllocatorPointer()
    {
        if (active()) allocator->release();
    }

    void setActive(bool act)
    {
        // Two cases
        if (active() && !act)
        {
            allocator->release();
            setBits(allocator, InactiveBit);
        }
        else if (!active() && act)
        {
            resetBits(allocator, InactiveBit);
            allocator->retain();
        }
    }

    explicit operator bool() const { return active(); }
    bool active() const { return allocator && !hasOneOf(allocator, InactiveBit); }
};

#define INHERIT_ALLOCATOR_CTORS(Derived, ...)                       \
    Derived() : AllocatorPointer<__VA_ARGS__>() {}                  \
    Derived(__VA_ARGS__ &allocator, bool active = true)             \
        : AllocatorPointer<__VA_ARGS__>(allocator, active) {}       \
    Derived(const Derived &o) : AllocatorPointer<__VA_ARGS__>(o) {} \
    Derived &operator=(const Derived &o)                            \
    {                                                               \
        if (this->active()) this->allocator->release();             \
        this->allocator = o.allocator;                              \
        if (this->active()) this->allocator->retain();              \
        return *this;                                               \
    }                                                               \
    Derived(Derived &&o) : AllocatorPointer<__VA_ARGS__>(o) {}      \
    Derived &operator=(Derived &&o)                                 \
    {                                                               \
        std::swap(this->allocator, o.allocator);                    \
        return *this;                                               \
    }
