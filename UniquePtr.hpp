#ifndef MY_BICYCLES_UNIQUE_PTR_H
#define MY_BICYCLES_UNIQUE_PTR_H

#include "Deleter.hpp"

#include <utility>

namespace mybicycles
{

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr
{
public:
    UniquePtr() noexcept;
    explicit UniquePtr(T* ptr) noexcept;

    UniquePtr(const UniquePtr<T, Deleter>& rhs) = delete;
    UniquePtr<T, Deleter>& operator= (const UniquePtr<T, Deleter>& rhs) = delete;
    UniquePtr(UniquePtr<T, Deleter>&& rhs) noexcept;
    UniquePtr<T, Deleter>& operator= (UniquePtr<T, Deleter>&& rhs) noexcept;

    ~UniquePtr();

    T* release() noexcept;
    void reset(T* ptr = nullptr) noexcept;
    void reset(UniquePtr<T, Deleter>&& rhs) noexcept;
    void swap(UniquePtr<T, Deleter>& rhs) noexcept;

    T* get() const noexcept;
    explicit operator bool() const noexcept;
    bool operator== (std::nullptr_t) const noexcept;
    bool operator!= (std::nullptr_t) const noexcept;
    T& operator* () const;
    T* operator-> () const noexcept;

    static void swap(UniquePtr<T, Deleter>& lhs, UniquePtr<T, Deleter>& rhs) noexcept;

private:
    T* mPtr;
};

template <typename T, typename Deleter>
inline UniquePtr<T, Deleter>::UniquePtr() noexcept :
    mPtr(nullptr)
{
}

template <typename T, typename Deleter>
inline UniquePtr<T, Deleter>::UniquePtr(T* ptr) noexcept :
    mPtr(ptr)
{
}

template <typename T, typename Deleter>
inline UniquePtr<T, Deleter>::UniquePtr(UniquePtr<T, Deleter>&& rhs) noexcept :
    mPtr(nullptr)
{
    mPtr = rhs.mPtr;
    rhs.mPtr = nullptr;
}

template <typename T, typename Deleter>
inline UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator= (UniquePtr<T, Deleter>&& rhs) noexcept
{
    if (this != &rhs)
    {
        reset();
        mPtr = rhs.mPtr;
        rhs.mPtr = nullptr;
    }
    return *this;
}

template <typename T, typename Deleter>
inline UniquePtr<T, Deleter>::~UniquePtr()
{
    reset();
}

template <typename T, typename Deleter>
inline T* UniquePtr<T, Deleter>::release() noexcept
{
    T* tmp = mPtr;
    mPtr = nullptr;
    return tmp;
}

template <typename T, typename Deleter>
inline void UniquePtr<T, Deleter>::reset(T* ptr) noexcept
{
    T* tmp = mPtr;
    mPtr = ptr;
    Deleter::deletePtr(tmp);
}

template <typename T, typename Deleter>
inline void UniquePtr<T, Deleter>::reset(UniquePtr<T, Deleter>&& rhs) noexcept
{
    operator=(std::move(rhs));
}

template <typename T, typename Deleter>
inline void UniquePtr<T, Deleter>::swap(UniquePtr<T, Deleter>& rhs) noexcept
{
    // Self-assignment is safe here
    T* tmp = mPtr;
    mPtr = rhs.mPtr;
    rhs.mPtr = tmp;
}

template <typename T, typename Deleter>
inline T* UniquePtr<T, Deleter>::get() const noexcept
{
    return mPtr;
}

template <typename T, typename Deleter>
inline UniquePtr<T, Deleter>::operator bool() const noexcept
{
    return mPtr != nullptr;
}

template <typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator==(std::nullptr_t) const noexcept
{
    return mPtr == nullptr;
}

template <typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator!=(std::nullptr_t) const noexcept
{
    return mPtr != nullptr;
}

template <typename T, typename Deleter>
inline T& UniquePtr<T, Deleter>::operator* () const
{
    // Undefined behavior if mPtr is nullptr
    // May throw if mPtr's operator* throws
    return *mPtr;
}

template <typename T, typename Deleter>
inline T* UniquePtr<T, Deleter>::operator-> () const noexcept
{
    // Undefined behavior if mPtr is nullptr
    return mPtr;
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::swap(UniquePtr<T, Deleter>& lhs, UniquePtr<T, Deleter>& rhs) noexcept
{
    // Self-assignment is safe here
    T* tmp = lhs.mPtr;
    lhs.mPtr = rhs.mPtr;
    rhs.mPtr = tmp;
}

} // mybicycles

#endif /* MY_BICYCLES_UNIQUE_PTR_H */
