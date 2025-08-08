#pragma once

#include "SharedPtr.hpp"

namespace mybicycles
{

template <typename T>
class EnableSharedFromThis
{
public:
    EnableSharedFromThis() noexcept :
        mWeakPtr()
    {}

    void enableSharedFromThis(SharedPtr<T> sp)
    {
        mWeakPtr = WeakPtr<T>(sp);
    }

    SharedPtr<T> getSharedFromThis()
    {
        return mWeakPtr.lock();
    }

private:
    WeakPtr<T> mWeakPtr;
};

} // mybicycles
