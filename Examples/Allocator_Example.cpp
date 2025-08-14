#include "Allocator_Example.hpp"
#include "BicycleImpl.hpp"

#include <vector>
#include <list>
#include <map>

void testMySimpleAllocator()
{
    using mybicycles::BicycleImpl;
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    {
        const int BICYCLE_LOGGING = true; // to see when Bicycle's ctor & dtor are called
        std::cout << "=======================std::vector:=======================" << std::endl;
        std::vector<BicycleImpl, MySimpleAllocator<BicycleImpl>> v;
        v.push_back({"Bicycle1", BICYCLE_LOGGING});
        v.push_back({"Bicycle2", BICYCLE_LOGGING});
        v.push_back({"Bicycle3", BICYCLE_LOGGING});
        v.push_back({"Bicycle4", BICYCLE_LOGGING});
        v.push_back({"Bicycle5", BICYCLE_LOGGING});
    }

    {
        std::cout << "=======================std::list:=======================" << std::endl;
        std::list<BicycleImpl, MySimpleAllocator<BicycleImpl>> l;
        l.push_back({"Bicycle1"});
        l.push_back({"Bicycle2"});
        l.push_back({"Bicycle3"});
    }

    {
        std::cout << "=======================std::map:=======================" << std::endl;
        std::map<int, BicycleImpl, std::less<int>, MySimpleAllocator<std::pair<const int, BicycleImpl>>> m;
        m.emplace(1, "Bicycle1");
        m.emplace(2, "Bicycle2");
        m.emplace(3, "Bicycle3");
    }

    std::cout << "============================================================= " << std::endl;
}
