#pragma once

#include <iostream>

/*
NOTES:

An allocator is used to acquire/release memory and to construct/destroy the elements in that memory.
The type must meet the requirements of Allocator.

Requirements for a custom allocator:
- define a type alias for value_type
- default ctor
- copy-constructible
- equality operator
- allocate method
- deallocate method
- rebind mechanism

All the requirements:
https://en.cppreference.com/w/cpp/named_req/Allocator.html

The implementation of many allocator requirements is optional because all AllocatorAwareContainer-s
access allocators indirectly through std::allocator_traits, and std::allocator_traits supplies the
default implementation of those requirements.

Purpose of rebind:
C++ Standard Library containers, like std::list or std::map, often need to allocate memory for
internal structures (e.g., list nodes, tree nodes) that are different from the user-provided
element type. The rebind member template within an allocator provides a standardized way to create
an allocator instance for these different types.
*/

void testMySimpleAllocator();

template <typename T>
class MySimpleAllocator
{
public:
    using value_type = T;
    using size_type = size_t;

    MySimpleAllocator() noexcept = default;
    MySimpleAllocator(const MySimpleAllocator& rhs) noexcept = default;
    MySimpleAllocator& operator=(const MySimpleAllocator& rhs) noexcept = default;
    ~MySimpleAllocator() = default;

    T* allocate(const size_t n)
    {
        std::cout << __PRETTY_FUNCTION__ << " | Allocating num of objects: " << n << std::endl;
        if (0 == n)
        {
            throw std::bad_alloc();
        }

        T* ret = static_cast<T*>(::operator new(n * sizeof(T)));
        return ret;
    }

    void deallocate(T* mem, const size_t n)
    {
        std::cout << __PRETTY_FUNCTION__ << " | Deallocating num of objects: " << n << std::endl;
        ::operator delete(mem, n);
    }

    template <typename U>
    struct rebind
    {
        using other = MySimpleAllocator<U>;
    };
};
