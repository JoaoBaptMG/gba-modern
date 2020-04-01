//--------------------------------------------------------------------------------
// ObjectStack.hpp
//--------------------------------------------------------------------------------
// Provides a class that can effectively allocate objects derived from a
// base class and expose them as pointers from that base
//--------------------------------------------------------------------------------
#pragma once

#include "type_traits.hpp"
#include "gba-assert.hpp"
#include <cstdint>
#include <cstddef>
#include <algorithm>

template <typename Base, std::size_t TotalSize, std::size_t MaxNumObjects>
class ObjectStack final
{
    static_assert(std::has_virtual_destructor_v<Base>,
        "ObjectStack will not work properly without virtual destructors!");

    struct Object final { Base* ptr; std::size_t size; };

    // Storage for the data
    alignas(void*) std::byte _storage[TotalSize];

    // Size, description of objects and "next free pointer"
    std::size_t _numObjects;
    Object _objects[MaxNumObjects];
    std::byte* _nextFreePointer;

    // Disable copy and moving
    ObjectStack(const ObjectStack&) = delete;
    ObjectStack(ObjectStack&&) = delete;

    ObjectStack& operator=(const ObjectStack&) = delete;
    ObjectStack& operator=(ObjectStack&&) = delete;

public:
    // Constructor
    ObjectStack() : _numObjects(0), _nextFreePointer(_storage) {}

    // Push an object to stack
    template <typename Derived, typename... Args>
    void push(Args&&... args)
    {
        static_assert(std::is_base_of_v<Base, Derived>,
            "The Derived class must be derived from Base!");
        static_assert(!is_virtual_base_of_v<Base, Derived>,
            "Virtual bases do not work with ObjectStack!");

        ASSERT(_numObjects < MaxNumObjects);
        ASSERT(_nextFreePointer - _storage + sizeof(Derived) <= TotalSize);

        // Construct the derived class in place
        auto pointer = new (_nextFreePointer) Derived(std::forward<Args>(args)...);

        // Store it into the new object - the implicit static_cast will add the
        // correct offset to the pointer if necessary
        _objects[_numObjects].ptr = pointer;

        // Store the size of the object
        _objects[_numObjects].size = sizeof(Derived);

        // And finally update the data
        _numObjects++;
        _nextFreePointer += sizeof(Derived);
    }

    // Peek the top object from the stack - assertion (but otherwise undefined behavior) if the stack is empty
    Base* top()
    { 
        ASSERT(!empty());
        return _objects[_numObjects-1].ptr;
    }

    // Is the stack empty?
    bool empty() { return _numObjects == 0; }

    // Pop the last object from the stack
    void pop()
    {
        ASSERT(!empty());

        _numObjects--;

        // Call the destruction function
        _objects[_numObjects].ptr->~Base();

        // Update the next free pointer
        _nextFreePointer -= _objects[_numObjects].size;
    }

    void clear()
    {
        for (std::intmax_t i = _numObjects - 1; i > 0; i--)
            _objects[i].ptr->~Base();

        _nextFreePointer = _storage;
        _numObjects = 0;
    }

    // Destructor - purge all objects
    ~ObjectStack()
    {
        for (std::intmax_t i = _numObjects - 1; i > 0; i--)
            _objects[i].ptr->~Base();
    }
};