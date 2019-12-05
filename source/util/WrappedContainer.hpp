//--------------------------------------------------------------------------------
// WrappedContainer.hpp
//--------------------------------------------------------------------------------
// Provides an object that wrap a container for iteration purposes, without
// providing access to the rest of the container though
//--------------------------------------------------------------------------------
#pragma once

template <typename T>
class WrappedContainer final
{
    // The container as a reference
    T& container;

public:
    WrappedContainer(T& container) : container(container) {}
    auto begin() { return container.begin(); }
    auto begin() const { return container.begin(); }
    auto cbegin() const { return container.cbegin(); }
    auto end() { return container.end(); }
    auto end() const { return container.end(); }
    auto cend() const { return container.cend(); }
};
