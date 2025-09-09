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
    constexpr bool BICYCLE_LOGGING = true; // to see when Bicycle's ctor & dtor are called
    constexpr size_t SEG_SIZE = 2048;

    using namespace mybicycles;
    using MyBicyclesAllocatorOnStack = MyAllocatorOnStack<BicycleImpl, SEG_SIZE>;
    using MyBicyclesPairAllocatorOnStack = MyAllocatorOnStack<std::pair<const int, BicycleImpl>, SEG_SIZE>;
    using MyBicyclesAllocatorNonOwning = MyAllocatorNonOwning<BicycleImpl>;
    using MyBicyclesPairAllocatorNonOwning = MyAllocatorNonOwning<std::pair<const int, BicycleImpl>>;

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    {

        std::cout << "=======================std::vector:======================" << std::endl;
        std::vector<BicycleImpl, MyBicyclesAllocatorOnStack> v1;
        v1.push_back({"Bicycle1", BICYCLE_LOGGING});
        v1.push_back({"Bicycle2", BICYCLE_LOGGING});
        v1.push_back({"Bicycle3", BICYCLE_LOGGING});
        v1.push_back({"Bicycle4", BICYCLE_LOGGING});
        v1.push_back({"Bicycle5", BICYCLE_LOGGING});

        char* seg = (char*)malloc(SEG_SIZE);
        SharedPtr<SimpleSegmentManager> ssm = makeShared<SimpleSegmentManager>(seg,
                                                                               SEG_SIZE);
        MyBicyclesAllocatorNonOwning myal(ssm, true);
        std::vector<BicycleImpl, MyBicyclesAllocatorNonOwning> v2(myal);
        v1.push_back({"BicycleA", BICYCLE_LOGGING});
        v1.push_back({"BicycleB", BICYCLE_LOGGING});
        v1.push_back({"BicycleC", BICYCLE_LOGGING});
        v1.push_back({"BicycleD", BICYCLE_LOGGING});
        v1.push_back({"BicycleE", BICYCLE_LOGGING});
        free(seg);
    }

    {
        std::cout << "=======================std::list:=======================" << std::endl;
        std::list<BicycleImpl, MyBicyclesAllocatorOnStack> l;
        l.push_back({"Bicycle1", BICYCLE_LOGGING});
        l.push_back({"Bicycle2", BICYCLE_LOGGING});
        l.push_back({"Bicycle3", BICYCLE_LOGGING});
    }

    {
        std::cout << "=======================std::map:=======================" << std::endl;
        std::map<int, BicycleImpl, std::less<int>, MyBicyclesPairAllocatorOnStack> m;
        m.insert(std::make_pair(1, BicycleImpl("Bicycle1", BICYCLE_LOGGING)));
        m.emplace(std::make_pair(2, BicycleImpl("Bicycle2", BICYCLE_LOGGING)));
        m.emplace(std::make_pair(3, BicycleImpl("Bicycle3", BICYCLE_LOGGING)));
    }

    std::cout << "=======================std::map:=======================" << std::endl;
}
