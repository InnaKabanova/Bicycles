#pragma once

#include "MyAllocatorBase.hpp"
#include "SimpleSegmentManager.hpp"

namespace mybicycles
{

/**
 * Custom allocator for STL containers. Allocates chunks of memory from the segment it owns.
 * The segment is on stack and is of size @SEG_SIZE.
 */
template <typename T, size_t SEG_SIZE, typename SegmentManagerType = SimpleSegmentManager>
class MyAllocatorOnStack : public MyAllocatorBase<T, SegmentManagerType>
{
public:
    // Rebind mechanism
    template <typename U>
    struct rebind
    {
        using other = MyAllocatorOnStack<U, SEG_SIZE, SegmentManagerType>;
    };

    MyAllocatorOnStack(bool loggingOn = false) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(loggingOn)
    {
        memset(mDefaultSegment, '0', SEG_SIZE);
        MyAllocatorBase<T, SegmentManagerType>::setSegmentManager(makeShared<SegmentManagerType>(mDefaultSegment, SEG_SIZE));
    }

    MyAllocatorOnStack(const MyAllocatorOnStack& rhs) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << mWarning << std::endl;
    }

    MyAllocatorOnStack(MyAllocatorOnStack&& rhs) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(std::move(rhs))
    {
        std::cout << __PRETTY_FUNCTION__ << mWarning << std::endl;
    }

    // Casting ctor
    template <typename U>
    MyAllocatorOnStack(const MyAllocatorOnStack<U, SEG_SIZE, SegmentManagerType>& rhs) noexcept :
        MyAllocatorBase<T, SegmentManagerType>(rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << mWarning << std::endl;
    }

    MyAllocatorOnStack& operator=(const MyAllocatorOnStack& rhs) noexcept
    {
        if (this != &rhs)
        {
            MyAllocatorBase<T, SegmentManagerType>::operator=(rhs);
            std::cout << __PRETTY_FUNCTION__ << mWarning << std::endl;
        }
        return *this;
    }

    MyAllocatorOnStack& operator=(MyAllocatorOnStack&& rhs) noexcept
    {
        if (this != &rhs)
        {
            MyAllocatorBase<T, SegmentManagerType>::operator=(std::move(rhs));
            std::cout << __PRETTY_FUNCTION__ << mWarning << std::endl;
        }
        return *this;
    }

    ~MyAllocatorOnStack() = default;

private:
    char mDefaultSegment[SEG_SIZE];
    const char* mWarning = "!!! WARNING: We are discarding usage of our own segment on stack and we'll be"
                           " allocating from rhs's segment from now on";
};

} // mybicycles
