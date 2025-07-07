#ifndef MY_BICYCLES_DELETER_H
#define MY_BICYCLES_DELETER_H

#include <iostream>

namespace mybicycles
{

template <typename T>
class DefaultDeleter
{
public:
    DefaultDeleter() = default;
    DefaultDeleter(const DefaultDeleter& rhs) = delete;
    DefaultDeleter& operator= (const DefaultDeleter& rhs) = delete;
    ~DefaultDeleter() = default;

    void operator() (T* ptr) const noexcept
    {
        delete ptr;
    }
};

template <typename T>
class ArrayDeleter
{
public:
    ArrayDeleter() = default;
    ArrayDeleter(const ArrayDeleter& rhs) = delete;
    ArrayDeleter& operator= (const ArrayDeleter& rhs) = delete;
    ~ArrayDeleter() = default;

    void operator() (T* ptr) const noexcept
    {
        delete[] ptr;
    }
};

template <typename T>
class CDeleter
{
public:
    CDeleter() = default;
    CDeleter(const CDeleter& rhs) = delete;
    CDeleter& operator= (const CDeleter& rhs) = delete;
    ~CDeleter() = default;

    void operator() (T* ptr) const noexcept
    {
        // Debug prints
        std::cout << "I am a custom deleter" << std::endl;
        if (ptr == nullptr)
        {
            std::cout << "Submitted resource is already null" << std::endl;
        }

        free(ptr);
    }
};

} /* mybicycles */

#endif /* MY_BICYCLES_DELETER_H */
