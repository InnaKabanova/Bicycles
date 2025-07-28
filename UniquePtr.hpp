#ifndef MY_BICYCLES_UNIQUE_PTR_H
#define MY_BICYCLES_UNIQUE_PTR_H

#include "Deleter.hpp"

#include <sstream>
#include <utility>

namespace mybicycles
{

/**
 * Smart pointer with semantics of exclusive ownership over the held resource.
 */
template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr
{
public:
    UniquePtr() noexcept;
    explicit UniquePtr(T* ptr) noexcept;

    UniquePtr(const UniquePtr& rhs) = delete;
    UniquePtr& operator= (const UniquePtr& rhs) = delete;
    UniquePtr(UniquePtr&& rhs) noexcept;
    UniquePtr& operator= (UniquePtr&& rhs) noexcept;

    ~UniquePtr();

    T* release() noexcept;
    void reset(T* ptr = nullptr) noexcept;
    void reset(UniquePtr&& rhs) noexcept;
    void swap(UniquePtr& rhs) noexcept;

    T* get() const noexcept;

    T& operator* () const;
    T* operator-> () const noexcept;

    explicit operator bool() const noexcept;

    bool operator== (std::nullptr_t) const noexcept;
    bool operator== (const UniquePtr& rhs) const noexcept;
    bool operator!= (std::nullptr_t) const noexcept;
    bool operator!= (const UniquePtr& rhs) const noexcept;
    bool operator< (const UniquePtr& rhs) const noexcept;
    bool operator<= (const UniquePtr& rhs) const noexcept;
    bool operator> (const UniquePtr& rhs) const noexcept;
    bool operator>= (const UniquePtr& rhs) const noexcept;

    static void swap(UniquePtr& lhs, UniquePtr& rhs) noexcept;

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
inline bool UniquePtr<T, Deleter>::operator==(const UniquePtr<T, Deleter>& rhs) const noexcept
{
    return mPtr == rhs.mPtr;
}

template <typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator!=(std::nullptr_t) const noexcept
{
    return mPtr != nullptr;
}

template <typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator!=(const UniquePtr<T, Deleter>& rhs) const noexcept
{
    return mPtr != rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator<(const UniquePtr<T, Deleter>& rhs) const noexcept
{
    return mPtr < rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator<=(const UniquePtr<T, Deleter>& rhs) const noexcept
{
    return mPtr < rhs.mPtr || mPtr == rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator>(const UniquePtr<T, Deleter>& rhs) const noexcept
{
    return mPtr > rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool UniquePtr<T, Deleter>::operator>=(const UniquePtr<T, Deleter>& rhs) const noexcept
{
    return mPtr > rhs.mPtr || mPtr == rhs.mPtr;
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::swap(UniquePtr<T, Deleter>& lhs, UniquePtr<T, Deleter>& rhs) noexcept
{
    // Self-assignment is safe here
    T* tmp = lhs.mPtr;
    lhs.mPtr = rhs.mPtr;
    rhs.mPtr = tmp;
}

template <typename T, typename Deleter>
std::ostream& operator<< (std::ostream& os, const UniquePtr<T, Deleter>& up)
{
    return (up ? os << up.get() : os << "nullptr");
}

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args)
{
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

} // bicycles

#endif /* MY_BICYCLES_UNIQUE_PTR_H */
