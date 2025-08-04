#pragma once

#include "Deleter.hpp"

#include <sstream>
#include <utility>

namespace mybicycles
{

template <typename T, typename Deleter>
class WeakPtr;

template <typename T>
struct ControlBlock
{
    ControlBlock() :
        mPtr(nullptr),
        mStrongUseCount(0),
        mWeakUseCount(0)
    {}

    ControlBlock(T* raw, unsigned int useCount) :
        mPtr(raw),
        mStrongUseCount(useCount),
        mWeakUseCount(0)
    {}

    ~ControlBlock() = default;
    ControlBlock(const ControlBlock& rhs) = delete;

    T* mPtr;
    unsigned int mStrongUseCount;
    unsigned int mWeakUseCount;
};

/**
 * Smart pointer with semantics of shared ownership over the held resource.
 * CAUTION: not thread-safe
 */
template <typename T, typename Deleter = DefaultDeleter<T>>
class SharedPtr
{
    friend class WeakPtr<T, Deleter>;

public:
    SharedPtr() noexcept;
    explicit SharedPtr(T* raw) noexcept;

    SharedPtr(const SharedPtr& rhs) noexcept;
    SharedPtr& operator= (const SharedPtr& rhs) noexcept;
    SharedPtr(SharedPtr&& rhs) noexcept;
    SharedPtr& operator= (SharedPtr&& rhs) noexcept;

    ~SharedPtr();

    void reset(T* rhs = nullptr) noexcept;
    void swap(SharedPtr& rhs) noexcept;

    T* get() const noexcept;

    T& operator* () const;
    T* operator-> () const noexcept;

    explicit operator bool() const noexcept;

    bool operator== (std::nullptr_t) const noexcept;
    bool operator== (const SharedPtr& rhs) const noexcept;
    bool operator!= (std::nullptr_t) const noexcept;
    bool operator!= (const SharedPtr& rhs) const noexcept;
    bool operator< (const SharedPtr& rhs) const noexcept;
    bool operator<= (const SharedPtr& rhs) const noexcept;
    bool operator> (const SharedPtr& rhs) const noexcept;
    bool operator>= (const SharedPtr& rhs) const noexcept;

    unsigned useCount() const noexcept;
    bool isUnique() const noexcept;

    static void swap(SharedPtr& lhs, SharedPtr& rhs);

private:
    void incrStrongUseCount() noexcept;

    ControlBlock<T>* mCb;
};

/**
 * Smart pointer with non-owning ("weak") reference to its held resource.
 * CAUTION: not thread-safe
 */
template <typename T, typename Deleter = DefaultDeleter<T>>
class WeakPtr
{
public:
    WeakPtr() noexcept;
    WeakPtr(const SharedPtr<T, Deleter>& sp) noexcept;

    WeakPtr(const WeakPtr& rhs) noexcept;
    WeakPtr& operator= (const WeakPtr& rhs) noexcept;
    WeakPtr(WeakPtr&& rhs) noexcept;
    WeakPtr& operator= (WeakPtr&& rhs) noexcept;

    ~WeakPtr() noexcept;

    void reset() noexcept;

    unsigned int useCount() const noexcept;
    bool isExpired() const noexcept;
    SharedPtr<T> lock() noexcept;

    static void swap(WeakPtr& lhs, WeakPtr& rhs);

private:
    void incrWeakUseCount() noexcept;

    ControlBlock<T>* mCb;
};

//--------------------------------------------------------------------------------------------------

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr() noexcept :
    mCb(nullptr)
{
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr(T* raw) noexcept :
    mCb(new ControlBlock<T>(raw, 1))
{
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::~SharedPtr()
{
    reset();
}

template<typename T, typename Deleter>
inline void SharedPtr<T, Deleter>::incrStrongUseCount() noexcept
{
    if (mCb)
    {
        mCb->mStrongUseCount++;
    }
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr(const SharedPtr<T, Deleter>& rhs) noexcept :
    mCb(rhs.mCb)
{
    incrStrongUseCount();
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>& SharedPtr<T, Deleter>::operator=(const SharedPtr<T, Deleter>& rhs) noexcept
{
    if (this != &rhs)
    {
        if (mCb)
        {
            // Free local held resource
            reset();
        }

        mCb = rhs.mCb;
        incrStrongUseCount();
    }
    return *this;
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr(SharedPtr<T, Deleter>&& rhs) noexcept :
    mCb(rhs.mCb)
{
    rhs.mCb = nullptr;
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>& SharedPtr<T, Deleter>::operator=(SharedPtr<T, Deleter>&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (mCb)
        {
            reset(); // free local held resource
        }

        mCb = rhs.mCb;
        rhs.mCb = nullptr;
    }
    return *this;
}

template<typename T, typename Deleter>
inline void SharedPtr<T, Deleter>::reset(T* rhs) noexcept
{
    // Free local held resource if any
    if (mCb)
    {
        mCb->mStrongUseCount--;
        if (0 == mCb->mStrongUseCount)
        {
            Deleter::deletePtr(mCb->mPtr);

            if (0 == mCb->mWeakUseCount)
            {
                delete mCb;
            }
        }
        mCb = nullptr;
    }

    if (rhs != nullptr)
    {
        mCb = new ControlBlock<T>(rhs, 1);
    }
}

template<typename T, typename Deleter>
inline void SharedPtr<T, Deleter>::swap(SharedPtr<T, Deleter>& rhs) noexcept
{
    SharedPtr<T, Deleter>::swap(*this, rhs);
}

template<typename T, typename Deleter>
inline T* SharedPtr<T, Deleter>::get() const noexcept
{
    if (!mCb)
    {
        return nullptr;
    }

    return mCb->mPtr;
}

template<typename T, typename Deleter>
inline T& SharedPtr<T, Deleter>::operator*() const
{
    // Undefined behavior if mCb or mPtr is nullptr
    // May throw if mPtr's operator* throws
    return *(mCb->mPtr);
}

template<typename T, typename Deleter>
inline T* SharedPtr<T, Deleter>::operator-> () const noexcept
{
    // Undefined behavior if mCb or mPtr is nullptr
    return mCb->mPtr;
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::operator bool() const noexcept
{
    if (!mCb)
    {
        return false;
    }

    return mCb->mPtr != nullptr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator==(std::nullptr_t) const noexcept
{
    if (!mCb)
    {
        return true;
    }

    return mCb->mPtr == nullptr;
}

template <typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator==(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    if (!mCb)
    {
        return rhs.mCb == nullptr;
    }

    return mCb->mPtr == rhs.mCb->mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator!=(std::nullptr_t) const noexcept
{
    if (!mCb)
    {
        return false;
    }

    return mCb->mPtr != nullptr;
}

template <typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator!=(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    if (!mCb)
    {
        return rhs.mCb != nullptr;
    }

    return mCb->mPtr != rhs.mCb->mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator<(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    if (!mCb)
    {
        return false;
    }

    return mCb->mPtr < rhs.mCb->mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator<=(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    if (!mCb)
    {
        return false;
    }

    return mCb->mPtr < rhs.mCb->mPtr || mCb->mPtr == rhs.mCb->mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator>(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    if (!mCb)
    {
        return false;
    }

    return mCb->mPtr > rhs.mCb->mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator>=(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    if (!mCb)
    {
        return false;
    }

    return mCb->mPtr > rhs.mCb->mPtr || mCb->mPtr == rhs.mCb->mPtr;
}

template<typename T, typename Deleter>
inline unsigned int SharedPtr<T, Deleter>::useCount() const noexcept
{
    if (!mCb)
    {
        return 0;
    }

    return mCb->mStrongUseCount;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::isUnique() const noexcept
{
    if (!mCb)
    {
        return true;
    }

    return mCb->mStrongUseCount == 1;
}

template<typename T, typename Deleter>
inline void SharedPtr<T, Deleter>::swap(SharedPtr<T, Deleter>& lhs, SharedPtr<T, Deleter>& rhs)
{
    if (&lhs != &rhs)
    {
        SharedPtr<T, Deleter> temp(lhs);
        lhs = rhs;
        rhs = temp;
    }
}

template <typename T, typename Deleter>
std::ostream& operator<< (std::ostream& os, const SharedPtr<T, Deleter>& sp)
{
    return (sp ? os << sp.get() : os << "nullptr");
}

//--------------------------------------------------------------------------------------------------

template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>::WeakPtr() noexcept :
    mCb(nullptr)
{
}

template<typename T, typename Deleter>
inline void WeakPtr<T, Deleter>::incrWeakUseCount() noexcept
{
    if (mCb)
    {
        mCb->mWeakUseCount++;
    }
}


template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>::WeakPtr(const SharedPtr<T, Deleter>& sp) noexcept :
    mCb(sp.mCb)
{
    incrWeakUseCount();
}

template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>::WeakPtr(const WeakPtr& rhs) noexcept :
    mCb(rhs.mCb)
{
    incrWeakUseCount();
}

template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>& WeakPtr<T, Deleter>::operator=(const WeakPtr& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();
        mCb = rhs.mCb;
        incrWeakUseCount();
    }
    return *this;
}

template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>::WeakPtr(WeakPtr&& rhs) noexcept :
    mCb(rhs.mCb)
{
    incrWeakUseCount(); // to avoid destruction of expired control block if any
    rhs.reset();
}

template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>& WeakPtr<T, Deleter>::operator=(WeakPtr&& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();
        mCb = rhs.mCb;
        incrWeakUseCount(); // to avoid destruction of expired control block if any
        rhs.reset();
    }
    return *this;
}

template<typename T, typename Deleter>
inline WeakPtr<T, Deleter>::~WeakPtr() noexcept
{
    reset();
}

template<typename T, typename Deleter>
inline void WeakPtr<T, Deleter>::reset() noexcept
{
    if (mCb)
    {
        mCb->mWeakUseCount--;
        if (0 == mCb->mStrongUseCount && 0 == mCb->mWeakUseCount)
        {
            delete mCb;
        }
    }
    mCb = nullptr;
}

template<typename T, typename Deleter>
inline unsigned int WeakPtr<T, Deleter>::useCount() const noexcept
{
    return (mCb == nullptr ? 0 : mCb->mStrongUseCount);
}

template<typename T, typename Deleter>
inline bool WeakPtr<T, Deleter>::isExpired() const noexcept
{
    return (mCb == nullptr ? true : mCb->mStrongUseCount == 0);
}

template<typename T, typename Deleter>
inline SharedPtr<T> WeakPtr<T, Deleter>::lock() noexcept
{
    if (isExpired())
    {
        return SharedPtr<T, Deleter>();
    }
    else
    {
        SharedPtr<T, Deleter> ret;
        ret.mCb = mCb;
        ret.incrStrongUseCount();
        return ret;
    }
}

template<typename T, typename Deleter>
inline void WeakPtr<T, Deleter>::swap(WeakPtr& lhs, WeakPtr& rhs)
{
    WeakPtr tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

} // mybicycles
