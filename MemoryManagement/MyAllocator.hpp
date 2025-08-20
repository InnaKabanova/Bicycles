#pragma once

#include <cstring>
#include <iostream>

#include "SimpleSegmentManager.hpp"
#include "SharedPtr.hpp"

/*
NOTES:

In terms of STL, an allocator is used to acquire/release memory and to construct/destroy the
elements in that memory. A custom allocator for an STL container must meet the following
requirements:
- define a type alias for value_type
- default ctor
- copy-constructible
- equality operator
- allocate method
- deallocate method
- rebind mechanism

All the requirements are here: https://en.cppreference.com/w/cpp/named_req/Allocator.html

The implementation of many allocator requirements is optional because all AllocatorAwareContainer-s
access allocators indirectly through std::allocator_traits, and std::allocator_traits supplies the
default implementation of those requirements.

Purpose of rebind:
C++ Standard Library containers, like std::list or std::map, often need to allocate memory for
internal structures (e.g., list nodes, tree nodes) that are different from the user-provided
element type. The rebind member template within an allocator provides a standardized way to create
an allocator instance for those different types.
*/

namespace mybicycles
{

template <typename T, typename SegmentManagerType = SimpleSegmentManager>
class MyAllocator
{
public:
    using value_type = T;
    using size_type = size_t;

    template <typename U>
    struct rebind
    {
        using other = MyAllocator<U, SegmentManagerType>;
    };

    static const size_t DEFAULT_SEGMENT_SIZE; // bytes

//--------------------------------------------------------------------------------------------------
    T* allocate(const size_t n)
    {
        if (mLog)
        {
            std::cout << __PRETTY_FUNCTION__ << " | Allocating num of objects: " << n << std::endl;
        }
        if (0 == n || !mSegmentManager)
        {
            throw std::bad_alloc();
        }
        T* ret = static_cast<T*>(mSegmentManager->alloc(n * sizeof(T)));
        return ret;
    }

    void deallocate(T* mem, const size_t n)
    {
        if (mLog)
        {
            std::cout << __PRETTY_FUNCTION__ << " | Deallocating num of objects: " << n << std::endl;
        }
        if (mSegmentManager)
        {
            mSegmentManager->free(mem);
        }
    }

//--------------------------------------------------------------------------------------------------
    MyAllocator() noexcept :
        mSegmentManager(nullptr),
        mLog(true)
    {
        memset(mDefaultSegment, '0', DEFAULT_SEGMENT_SIZE);
        mSegmentManager = makeShared<SegmentManagerType>(mDefaultSegment/* base segment addr*/,
                                                         DEFAULT_SEGMENT_SIZE,
                                                         false/* no owning*/);
    }

    MyAllocator(SegmentManagerType segmentManager, bool loggingOn) noexcept :
        mSegmentManager(segmentManager),
        mLog(loggingOn)
    {
    }

    MyAllocator(const MyAllocator& rhs) noexcept :
        mSegmentManager(rhs.mSegmentManager),
        mLog(rhs.mLog)
    {
    }

    MyAllocator(MyAllocator&& rhs) noexcept :
        mSegmentManager(std::move(rhs.mSegmentManager)),
        mLog(rhs.mLog)
    {
        rhs.mLog = false;
    }

    MyAllocator& operator=(const MyAllocator& rhs) noexcept
    {
        if (this != &rhs)
        {
            mSegmentManager.reset();
            mSegmentManager = rhs.mSegmentManager;
            mLog = rhs.mLog;
        }
        return *this;
    }

    MyAllocator& operator=(MyAllocator&& rhs) noexcept
    {
        if (this != &rhs)
        {
            mSegmentManager.reset();
            mSegmentManager = std::move(rhs.mSegmentManager);
            mLog = rhs.mLog;
            rhs.mLog = false;
        }
        return *this;
    }

    ~MyAllocator() = default;

//--------------------------------------------------------------------------------------------------
private:
    char mDefaultSegment[DEFAULT_SEGMENT_SIZE];

    SharedPtr<SegmentManagerType> mSegmentManager;
    bool mLog;
};

template <typename T, typename SegmentManagerType>
const size_t MyAllocator<T, SegmentManagerType>::DEFAULT_SEGMENT_SIZE = 256; // bytes

} // mybicycles
