#pragma once

#include "MyAllocatorBase.hpp"
#include "SimpleSegmentManager.hpp"

namespace mybicycles
{

/**
 * Custom allocator for STL containers. Allocates chunks of memory from the segment it doesn't own.
 */
template <typename T, typename SegmentManagerType = SimpleSegmentManager>
class MyAllocatorNonOwning : public MyAllocatorBase<T, SegmentManagerType>
{
public:
    // Rebind mechanism
    template <typename U>
    struct rebind
    {
        using other = MyAllocatorNonOwning<U, SegmentManagerType>;
    };

    MyAllocatorNonOwning(bool loggingOn = false) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(loggingOn)
    {
    }

    MyAllocatorNonOwning(const SharedPtr<SegmentManagerType>& segmentManager, bool loggingOn = false) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(segmentManager, loggingOn)
    {
    }

    MyAllocatorNonOwning(const MyAllocatorNonOwning& rhs) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(rhs)
    {
    }

    MyAllocatorNonOwning(MyAllocatorNonOwning&& rhs) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(std::move(rhs))
    {
    }

    // Casting ctor
    template <typename U>
    MyAllocatorNonOwning(const MyAllocatorNonOwning<U, SegmentManagerType>& rhs) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(rhs)
    {
    }

    MyAllocatorNonOwning& operator=(const MyAllocatorNonOwning& rhs) noexcept
    {
        if (this != &rhs)
        {
            MyAllocatorBase<T, SegmentManagerType>::operator=(rhs);
        }
        return *this;
    }

    MyAllocatorNonOwning& operator=(MyAllocatorNonOwning&& rhs) noexcept
    {
        if (this != &rhs)
        {
            MyAllocatorBase<T, SegmentManagerType>::operator=(std::move(rhs));
        }
        return *this;
    }

    ~MyAllocatorNonOwning() = default;
};


} // mybicycles
