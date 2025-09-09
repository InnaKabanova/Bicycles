#pragma once

#include <cstring>
#include <iostream>

#include "SharedPtr.hpp"

/*
NOTES ON ALLOCATORS:

In terms of STL containers, an allocator is used to acquire/release memory and to construct/destroy
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

Implementation of many allocator requirements is optional because all AllocatorAwareContainer-s
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

/**
 * Common functionality for the MyAllocator* custom allocators.
 */
template <typename T, typename SegmentManagerType>
class MyAllocatorBase
{
public:
    using value_type = T;
    using size_type = size_t;

    T* allocate(const size_t n)
    {
        if (mLog)
        {
            std::cout << __PRETTY_FUNCTION__ << " | Allocating num of objects: " << n <<
                                                " | " << n * sizeof(T) << " bytes are required" << std::endl;
        }
        if (0 == n || !mSegmentManager)
        {
            throw std::bad_alloc();
        }

        size_t neededBytes = n * sizeof(T);
        void* mem = mSegmentManager->alloc(neededBytes);
        if (nullptr == mem)
        {
            std::string errMsg = "Segment large enough is not found (" + std::to_string(neededBytes)
                                                                       + " bytes were requested)";
            throw std::runtime_error(errMsg.c_str());
        }

        return static_cast<T*>(mem);
    }

    void deallocate(T* mem, const size_t n)
    {
        if (mLog)
        {
            std::cout << __PRETTY_FUNCTION__ << " | Deallocating num of objects: " << n <<
                                                " | " << n * sizeof(T) << " bytes are to be freed" << std::endl;
        }
        if (mSegmentManager)
        {
            mSegmentManager->free(mem);
        }
    }

    SharedPtr<SegmentManagerType> getSegmentManager() const
    {
        return mSegmentManager;
    }

    bool isLoggingEnabled() const
    {
        return mLog;
    }

protected:
    void setSegmentManager(const SharedPtr<SegmentManagerType>& segmentManager)
    {
        mSegmentManager = segmentManager;
    }

    MyAllocatorBase(bool loggingOn = false) noexcept :
        mSegmentManager(nullptr),
        mLog(loggingOn)
    {
    }

    MyAllocatorBase(const SharedPtr<SegmentManagerType>& segmentManager, bool loggingOn = false) noexcept :
        mSegmentManager(segmentManager),
        mLog(loggingOn)
    {
    }

    // Casting ctor
    template <typename U>
    MyAllocatorBase(const MyAllocatorBase<U, SegmentManagerType>& rhs) noexcept :
        mSegmentManager(rhs.getSegmentManager()),
        mLog(rhs.isLoggingEnabled())
    {
    }

    MyAllocatorBase(const MyAllocatorBase& rhs) noexcept :
        mSegmentManager(rhs.mSegmentManager),
        mLog(rhs.mLog)
    {
    }

    MyAllocatorBase(MyAllocatorBase&& rhs) noexcept :
        mSegmentManager(std::move(rhs.mSegmentManager)),
        mLog(rhs.mLog)
    {
        rhs.mSegmentManager = nullptr;
        rhs.mLog = false;
    }

    MyAllocatorBase& operator=(const MyAllocatorBase& rhs) noexcept
    {
        // this != &rhs check to be performed by Derived classes
        mSegmentManager.reset();
        mSegmentManager = rhs.mSegmentManager;
        mLog = rhs.mLog;
        return *this;
    }

    MyAllocatorBase& operator=(MyAllocatorBase&& rhs) noexcept
    {
        // this != &rhs check to be performed by Derived classes
        mSegmentManager.reset();
        mSegmentManager = std::move(rhs.mSegmentManager);
        mLog = rhs.mLog;
        rhs.mSegmentManager = nullptr;
        rhs.mLog = false;
        return *this;
    }

protected:
    SharedPtr<SegmentManagerType> mSegmentManager;
    bool mLog;
};

} // mybicycles
