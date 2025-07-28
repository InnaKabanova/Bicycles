#ifndef MY_BICYCLES_SHARED_PTR
#define MY_BICYCLES_SHARED_PTR

#include "Deleter.hpp"

#include <sstream>
#include <utility>

namespace mybicycles
{

template <typename T, typename Deleter = DefaultDeleter<T>>
class SharedPtr
{
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
    T* mPtr;
    unsigned int* mUseCount;
};

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr() noexcept :
    mPtr(nullptr),
    mUseCount(nullptr)
{
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr(T* raw) noexcept :
    mPtr(raw),
    mUseCount(new unsigned int(1))
{
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::~SharedPtr()
{
    reset();
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr(const SharedPtr<T, Deleter>& rhs) noexcept :
    mPtr(rhs.mPtr),
    mUseCount(rhs.mUseCount)
{
    if (mPtr != nullptr)
    {
        (*mUseCount)++;
    }
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>& SharedPtr<T, Deleter>::operator=(const SharedPtr<T, Deleter>& rhs) noexcept
{
    if (this != &rhs)
    {
        if (mPtr != rhs.mPtr)
        {
            // Free local held resource
            reset();
            mPtr = rhs.mPtr;
            mUseCount = rhs.mUseCount;
            (*mUseCount)++;
        }
    }
    return *this;
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::SharedPtr(SharedPtr<T, Deleter>&& rhs) noexcept :
    mPtr(rhs.mPtr),
    mUseCount(rhs.mUseCount)
{
    rhs.mPtr = nullptr;
    rhs.mUseCount = nullptr;
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>& SharedPtr<T, Deleter>::operator=(SharedPtr<T, Deleter>&& rhs) noexcept
{
    if (this != &rhs)
    {
        if (mPtr != rhs.mPtr)
        {
            reset(); // free local held resource
            mPtr = rhs.mPtr;
            mUseCount = rhs.mUseCount;
            rhs.mPtr = nullptr;
            rhs.mUseCount = nullptr;
        }
    }
    return *this;
}

template<typename T, typename Deleter>
inline void SharedPtr<T, Deleter>::reset(T* rhs) noexcept
{
    // Free local held resource if any
    if (mUseCount != nullptr)
    {
        (*mUseCount)--;
        if (0 == (*mUseCount))
        {
            delete mUseCount;
            mUseCount = nullptr;
            Deleter::deletePtr(mPtr);
        }
    }

    mPtr = nullptr;

    if (rhs != nullptr)
    {
        mUseCount = new unsigned int(1);
        mPtr = rhs;
    }
}

template<typename T, typename Deleter>
inline void SharedPtr<T, Deleter>::swap(SharedPtr<T, Deleter>& rhs) noexcept
{
    SharedPtr<T, Deleter>::swap(*this, rhs);
}

template<typename T, typename Deleter>
inline T *SharedPtr<T, Deleter>::get() const noexcept
{
    return mPtr;
}

template<typename T, typename Deleter>
inline T& SharedPtr<T, Deleter>::operator*() const
{
    // Undefined behavior if mPtr is nullptr
    // May throw if mPtr's operator* throws
    return *mPtr;
}

template<typename T, typename Deleter>
inline T* SharedPtr<T, Deleter>::operator ->() const noexcept
{
    // Undefined behavior if mPtr is nullptr
    return mPtr;
}

template<typename T, typename Deleter>
inline SharedPtr<T, Deleter>::operator bool() const noexcept
{
    return mPtr != nullptr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator==(std::nullptr_t) const noexcept
{
    return mPtr == nullptr;
}

template <typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator==(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    return mPtr == rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator!=(std::nullptr_t) const noexcept
{
    return mPtr != nullptr;
}

template <typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator!=(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    return mPtr != rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator<(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    return mPtr < rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator<=(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    return mPtr < rhs.mPtr || mPtr == rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator>(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    return mPtr > rhs.mPtr;
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::operator>=(const SharedPtr<T, Deleter>& rhs) const noexcept
{
    return mPtr > rhs.mPtr || mPtr == rhs.mPtr;
}

template<typename T, typename Deleter>
inline unsigned int SharedPtr<T, Deleter>::useCount() const noexcept
{
    return (mUseCount == nullptr ? 0 : *mUseCount);
}

template<typename T, typename Deleter>
inline bool SharedPtr<T, Deleter>::isUnique() const noexcept
{
    return (mUseCount == nullptr || *mUseCount == 1);
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

} // mybicycles

#endif /* MY_BICYCLES_SHARED_PTR */
