//--------------------------------------------------------------------------------
// AllocatorPointer.hpp
//--------------------------------------------------------------------------------
// RAII wrapper over any object that implements a simple allocator pattern
//--------------------------------------------------------------------------------

#pragma once

// An allocator class that uses CRTP to access its derived type
template <typename T>
class Allocator
{
    u16 refs;

protected:
    // Increases the reference counter
    void retain()
    {
        if (refs == 0)
            static_cast<T*>(this)->alloc();
        refs++;
    }

    // Decreases the reference counter
    void release()
    {
        refs--;
        if (refs == 0)
            static_cast<T*>(this)->clear();
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
protected:
    T *allocator;

public:
    AllocatorPointer() : allocator(nullptr) {}

    AllocatorPointer(T& allocator) : allocator(&allocator)
    {
        allocator.retain();
    }

    AllocatorPointer(const AllocatorPointer& o) : allocator(o.allocator)
    {
        if (allocator) allocator->retain();
    }

    AllocatorPointer& operator=(const AllocatorPointer& o)
    {
        if (allocator) allocator->release();
        allocator = o.allocator;
        if (allocator) allocator->retain();
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
        if (allocator) allocator->release();
    }

    explicit operator bool() const { return allocator; }
};

#define INHERIT_ALLOCATOR_CTORS(Derived, ...)                                     \
    Derived() : AllocatorPointer<__VA_ARGS__>() {}                                \
    Derived(__VA_ARGS__ &allocator) : AllocatorPointer<__VA_ARGS__>(allocator) {} \
    Derived(const Derived &o) : AllocatorPointer<__VA_ARGS__>(o) {}               \
    Derived &operator=(const Derived &o)                                          \
    {                                                                             \
        if (this->allocator) this->allocator->release();                          \
        this->allocator = o.allocator;                                            \
        if (this->allocator) this->allocator->retain();                           \
        return *this;                                                             \
    }                                                                             \
    Derived(Derived &&o) : AllocatorPointer<__VA_ARGS__>(o) {}                    \
    Derived &operator=(Derived &&o)                                               \
    {                                                                             \
        std::swap(this->allocator, o.allocator);                                  \
        return *this;                                                             \
    }
