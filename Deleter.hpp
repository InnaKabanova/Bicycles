#pragma once

#include <iostream>

namespace mybicycles
{

template <typename T>
class DefaultDeleter
{
public:
    static void deletePtr(T* ptr) noexcept
    {
        delete ptr;
    }
};

template <typename T>
class ArrayDeleter
{
public:
    static void deletePtr(T* ptr) noexcept
    {
        delete[] ptr;
    }
};

template <typename T>
class CDeleter
{
public:
    static void deletePtr(T* ptr) noexcept
    {
//        std::cout << "I am a custom deleter" << std::endl;
//        if (ptr == nullptr)
//        {
//            std::cout << "Submitted resource is already null" << std::endl;
//        }
        free(ptr);
    }
};

} /* mybicycles */
