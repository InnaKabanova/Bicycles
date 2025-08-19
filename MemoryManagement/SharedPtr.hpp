#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <type_traits>

namespace mybicycles
{

template <typename T>
class WeakPtr;

template <typename T>
class EnableSharedFromThis;

//--------------------------------------------------------------------------------------------------
template <typename T>
struct ControlBlock
{
    static const std::function<void(T*)> DEFAULT_DELETER;

    ControlBlock() :
        mPtr(nullptr),
        mStrongUseCount(0),
        mWeakUseCount(0),
        mDeleter(DEFAULT_DELETER)
    {}

    ControlBlock(T* raw, unsigned int strongUseCount, std::function<void(T*)> deleter = DEFAULT_DELETER) :
        mPtr(raw),
        mStrongUseCount(strongUseCount),
        mWeakUseCount(0),
        mDeleter(deleter)
    {}

    ~ControlBlock() = default;
    ControlBlock(const ControlBlock& rhs) = delete;

    T* mPtr;
    unsigned int mStrongUseCount;
    unsigned int mWeakUseCount;
    std::function<void(T*)> mDeleter;
};

template <typename T>
const std::function<void(T*)> ControlBlock<T>::DEFAULT_DELETER = [](T* ptr){delete ptr;};

//--------------------------------------------------------------------------------------------------
/**
 * Smart pointer with semantics of shared ownership over the held resource.
 * CAUTION: not thread-safe
 */
template <typename T>
class SharedPtr
{
    friend class WeakPtr<T>;

public:
    SharedPtr() noexcept;
    explicit SharedPtr(T* raw) noexcept;
    template<typename Deleter>
    SharedPtr(T* raw, Deleter deleter) noexcept;

    SharedPtr(const SharedPtr& rhs) noexcept;
    SharedPtr& operator= (const SharedPtr& rhs) noexcept;
    SharedPtr(SharedPtr&& rhs) noexcept;
    SharedPtr& operator= (SharedPtr&& rhs) noexcept;

    ~SharedPtr();

    void reset(T* rhs = nullptr) noexcept;
    template<typename Deleter>
    void reset(T* rhs, Deleter deleter) noexcept;

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
    void deleteResource() noexcept;

    /**
     * Initialization method for types which are not derived from @EnableSharedFromThis
     */
    template <typename U = T>
    typename std::enable_if<!std::is_base_of<EnableSharedFromThis<U>, U>::value>::type
    initResourceIfEnableSharedFromThis()
    {
        // Do nothing
    }
    /**
     * Initialization method for types which are derived from @EnableSharedFromThis
     */
    template <typename U = T>
    typename std::enable_if<std::is_base_of<EnableSharedFromThis<U>, U>::value>::type
    initResourceIfEnableSharedFromThis()
    {
        mCb->mPtr->mWeakThis = WeakPtr<T>(*this);
    }

    ControlBlock<T>* mCb;
};

//--------------------------------------------------------------------------------------------------
/**
 * Smart pointer with non-owning ("weak") reference to its held resource.
 * CAUTION: not thread-safe
 */
template <typename T>
class WeakPtr
{
public:
    WeakPtr() noexcept;
    WeakPtr(const SharedPtr<T>& sp) noexcept;

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

class BadWeakPtr : public std::exception
{
public:
    virtual const char* what() const noexcept override
    {
        return "No SharedPtr owns this object";
    }
};

/**
 * Utility class to allow a @SharedPtr-managed object to create an additional @SharedPtr to itself.
 * For that, the object must be of a class which is _publicly_ derived from EnableSharedFromThis.
 */
template <typename T>
class EnableSharedFromThis
{
    friend class SharedPtr<T>;

protected:
    EnableSharedFromThis() noexcept :
        mWeakThis()
    {
    }
    EnableSharedFromThis(const EnableSharedFromThis& rhs) noexcept = default;
    EnableSharedFromThis& operator= (const EnableSharedFromThis& rhs) noexcept = default;
    ~EnableSharedFromThis() = default;

public:
    /**
     * Initialization of a EnableSharedFromThis-derived object is responsibility of a @SharedPtr
     * managing it. If this object is not referenced by any @SharedPtr, then a call to
     * getSharedFromThis will result in the @BadWeakPtr exception.
     */
    SharedPtr<T> getSharedFromThis()
    {
        if (mWeakThis.isExpired())
        {
            throw BadWeakPtr();
        }
        return mWeakThis.lock();
    }

    /**
     * If user doesn't want to handle possible @BadWeakPtr exception, s/he can obtain a @WeakPtr via
     * getWeakFromThis and call @WeakPtr<T>::lock (and then check the obtained @SharedPtr for nullness).
     */
    WeakPtr<T> getWeakFromThis() noexcept
    {
        return mWeakThis;
    }

private:
    WeakPtr<T> mWeakThis;
};

//--------------------------------------------------------------------------------------------------
template<typename T>
inline SharedPtr<T>::SharedPtr() noexcept :
    mCb(nullptr)
{
}

template<typename T>
inline SharedPtr<T>::SharedPtr(T* raw) noexcept :
    mCb(new ControlBlock<T>(raw, 1))
{
    initResourceIfEnableSharedFromThis();
}

template<typename T>
template<typename Deleter>
inline SharedPtr<T>::SharedPtr(T* raw, Deleter deleter) noexcept :
    mCb(new ControlBlock<T>(raw, 1, deleter))
{
    initResourceIfEnableSharedFromThis();
}

template<typename T>
inline SharedPtr<T>::~SharedPtr()
{
    reset();
}

template<typename T>
inline void SharedPtr<T>::incrStrongUseCount() noexcept
{
    if (mCb)
    {
        mCb->mStrongUseCount++;
    }
}

template<typename T>
inline void SharedPtr<T>::deleteResource() noexcept
{
    // Free local held resource if any
    if (mCb)
    {
        mCb->mStrongUseCount--;
        if (0 == mCb->mStrongUseCount)
        {
            mCb->mDeleter(mCb->mPtr);
            if (0 == mCb->mWeakUseCount)
            {
                delete mCb;
            }
        }
        mCb = nullptr;
    }
}

template<typename T>
inline SharedPtr<T>::SharedPtr(const SharedPtr<T>& rhs) noexcept :
    mCb(rhs.mCb)
{
    incrStrongUseCount();
}

template<typename T>
inline SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();

        mCb = rhs.mCb;
        incrStrongUseCount();
    }
    return *this;
}

template<typename T>
inline SharedPtr<T>::SharedPtr(SharedPtr<T>&& rhs) noexcept :
    mCb(rhs.mCb)
{
    rhs.mCb = nullptr;
}

template<typename T>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();

        mCb = rhs.mCb;
        rhs.mCb = nullptr;
    }
    return *this;
}

template<typename T>
inline void SharedPtr<T>::reset(T* rhs) noexcept
{
    deleteResource();
    if (rhs != nullptr)
    {
        mCb = new ControlBlock<T>(rhs, 1);
    }
}

template<typename T>
template<typename Deleter>
inline void SharedPtr<T>::reset(T* raw, Deleter deleter) noexcept
{
    deleteResource();
    if (raw != nullptr)
    {
        mCb = new ControlBlock<T>(raw, 1, deleter);
    }
}

template<typename T>
inline void SharedPtr<T>::swap(SharedPtr<T>& rhs) noexcept
{
    SharedPtr<T>::swap(*this, rhs);
}

template<typename T>
inline T* SharedPtr<T>::get() const noexcept
{
    if (!mCb)
    {
        return nullptr;
    }

    return mCb->mPtr;
}

template<typename T>
inline T& SharedPtr<T>::operator*() const
{
    // Undefined behavior if mCb or mPtr is nullptr
    // May throw if mPtr's operator* throws
    return *get();
}

template<typename T>
inline T* SharedPtr<T>::operator-> () const noexcept
{
    // Undefined behavior if mCb or mPtr is nullptr
    return get();
}

template<typename T>
inline SharedPtr<T>::operator bool() const noexcept
{
    return get() != nullptr;
}

template<typename T>
inline bool SharedPtr<T>::operator==(std::nullptr_t) const noexcept
{
    return get() == nullptr;
}

template <typename T>
inline bool SharedPtr<T>::operator==(const SharedPtr<T>& rhs) const noexcept
{
    return get() == rhs.get();
}

template<typename T>
inline bool SharedPtr<T>::operator!=(std::nullptr_t) const noexcept
{
    return get() != nullptr;
}

template <typename T>
inline bool SharedPtr<T>::operator!=(const SharedPtr<T>& rhs) const noexcept
{
    return get() != rhs.get();
}

template<typename T>
inline bool SharedPtr<T>::operator<(const SharedPtr<T>& rhs) const noexcept
{
    return get() < rhs.get();
}

template<typename T>
inline bool SharedPtr<T>::operator<=(const SharedPtr<T>& rhs) const noexcept
{
    return get() <= rhs.get();
}

template<typename T>
inline bool SharedPtr<T>::operator>(const SharedPtr<T>& rhs) const noexcept
{
    return get() > rhs.get();
}

template<typename T>
inline bool SharedPtr<T>::operator>=(const SharedPtr<T>& rhs) const noexcept
{
    return get() >= rhs.get();
}

template<typename T>
inline unsigned int SharedPtr<T>::useCount() const noexcept
{
    if (!mCb)
    {
        return 0;
    }

    return mCb->mStrongUseCount;
}

template<typename T>
inline bool SharedPtr<T>::isUnique() const noexcept
{
    if (!mCb)
    {
        return true;
    }

    return mCb->mStrongUseCount == 1;
}

template<typename T>
inline void SharedPtr<T>::swap(SharedPtr<T>& lhs, SharedPtr<T>& rhs)
{
    std::swap(lhs.mCb, rhs.mCb);
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const SharedPtr<T>& sp)
{
    return (sp ? os << sp.get() : os << "nullptr");
}

/**
 * Unlike the SharedPtr's constructors, makeShared doesn't provide an option to submit a custom
 * deleter.
 */
template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args)
{
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

//--------------------------------------------------------------------------------------------------
template<typename T>
inline WeakPtr<T>::WeakPtr() noexcept :
    mCb(nullptr)
{
}

template<typename T>
inline void WeakPtr<T>::incrWeakUseCount() noexcept
{
    if (mCb)
    {
        mCb->mWeakUseCount++;
    }
}

template<typename T>
inline WeakPtr<T>::WeakPtr(const SharedPtr<T>& sp) noexcept :
    mCb(sp.mCb)
{
    incrWeakUseCount();
}

template<typename T>
inline WeakPtr<T>::WeakPtr(const WeakPtr& rhs) noexcept :
    mCb(rhs.mCb)
{
    incrWeakUseCount();
}

template<typename T>
inline WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();
        mCb = rhs.mCb;
        incrWeakUseCount();
    }
    return *this;
}

template<typename T>
inline WeakPtr<T>::WeakPtr(WeakPtr&& rhs) noexcept :
    mCb(nullptr)
{
    std::swap(mCb, rhs.mCb);
}

template<typename T>
inline WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();
        std::swap(mCb, rhs.mCb);
    }
    return *this;
}

template<typename T>
inline WeakPtr<T>::~WeakPtr() noexcept
{
    reset();
}

template<typename T>
inline void WeakPtr<T>::reset() noexcept
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

template<typename T>
inline unsigned int WeakPtr<T>::useCount() const noexcept
{
    return (mCb == nullptr ? 0 : mCb->mStrongUseCount);
}

template<typename T>
inline bool WeakPtr<T>::isExpired() const noexcept
{
    return (mCb == nullptr ? true : mCb->mStrongUseCount == 0);
}

template<typename T>
inline SharedPtr<T> WeakPtr<T>::lock() noexcept
{
    if (isExpired())
    {
        return SharedPtr<T>();
    }
    else
    {
        SharedPtr<T> ret;
        ret.mCb = mCb;
        ret.incrStrongUseCount();
        return ret;
    }
}

template<typename T>
inline void WeakPtr<T>::swap(WeakPtr& lhs, WeakPtr& rhs)
{
    WeakPtr tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

} // mybicycles
