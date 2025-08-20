#pragma once

#include <cstring>
#include <iostream>

#include "SimpleSegmentManager.hpp"
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

    // Size of a memory segment this allocator can operate in case it was default-constructed and
    // thus was not provided a segment to manage.
    static const size_t DEFAULT_SEGMENT_SIZE; // bytes

//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
    MyAllocator() noexcept :
        mSegmentManager(nullptr),
        mLog(false)
    {
        memset(mDefaultSegment, '0', DEFAULT_SEGMENT_SIZE);
        mSegmentManager = makeShared<SegmentManagerType>(mDefaultSegment/* base segment addr */,
                                                         DEFAULT_SEGMENT_SIZE,
                                                         false/* no owning */);
    }

    MyAllocator(const SharedPtr<SegmentManagerType>& segmentManager, bool loggingOn = false) noexcept :
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
        rhs.mSegmentManager.reset();
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
            rhs.mSegmentManager = nullptr;
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

// Keeping this one very low, because it imposes a serious memory overhead for each MyAllocator object,
// either default-constructed or given a memory segment to manage
template <typename T, typename SegmentManagerType>
const size_t MyAllocator<T, SegmentManagerType>::DEFAULT_SEGMENT_SIZE = 128; // bytes

} // mybicycles
