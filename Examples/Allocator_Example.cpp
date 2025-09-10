#include "Allocator_Example.hpp"
#include "BicycleImpl.hpp"
#include "MemoryManagement/MyAllocatorOnStack.hpp"
#include "MemoryManagement/MyAllocatorNonOwning.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <map>

void testMyAllocators()
{
    constexpr bool BICYCLE_LOGGING = false; // to see when Bicycle's ctor & dtor are called
    constexpr size_t SEG_SIZE = 1024;

    using namespace mybicycles;
    using MyBicyclesAllocatorOnStack = MyAllocatorOnStack<BicycleImpl, SEG_SIZE>;
    using MyBicyclesPairAllocatorOnStack = MyAllocatorOnStack<std::pair<const int, BicycleImpl>, SEG_SIZE>;
    using MyBicyclesAllocatorNonOwning = MyAllocatorNonOwning<BicycleImpl>;
    using MyBicyclesPairAllocatorNonOwning = MyAllocatorNonOwning<std::pair<const int, BicycleImpl>>;

    std::cout << __PRETTY_FUNCTION__ << std::endl;

    {
        std::cout << "=======================std::vector:======================" << std::endl;

        constexpr bool ALLOC_LOGGING = true;
        MyBicyclesAllocatorOnStack myal(ALLOC_LOGGING);
        std::vector<BicycleImpl, MyBicyclesAllocatorOnStack> v1(myal);
        v1.push_back({"Bicycle1", BICYCLE_LOGGING});
        v1.push_back({"Bicycle2", BICYCLE_LOGGING});
        v1.push_back({"Bicycle3", BICYCLE_LOGGING});
        v1.push_back({"Bicycle4", BICYCLE_LOGGING});
        v1.clear();
        v1.shrink_to_fit();
    }

    {
        std::cout << "=======================std::list:=======================" << std::endl;
        MyBicyclesPairAllocatorOnStack myal2(true);
        std::list<BicycleImpl, MyBicyclesAllocatorOnStack> l(myal2);
        l.push_back({"Bicycle1", BICYCLE_LOGGING});
        l.push_back({"Bicycle2", BICYCLE_LOGGING});
        l.clear();
    }

    {
        std::cout << "=======================std::map:=======================" << std::endl;
        std::map<int, BicycleImpl, std::less<int>, MyBicyclesPairAllocatorOnStack> m;
        m.insert(std::make_pair(1, BicycleImpl("Bicycle1", BICYCLE_LOGGING)));
        m.emplace(std::make_pair(2, BicycleImpl("Bicycle2", BICYCLE_LOGGING)));
        m.emplace(std::make_pair(3, BicycleImpl("Bicycle3", BICYCLE_LOGGING)));
    }

    std::cout << "END END END END" << std::endl;
}
