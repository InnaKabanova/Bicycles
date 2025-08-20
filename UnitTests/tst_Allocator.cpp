#include <gtest/gtest.h>

#include "MockBicycle.hpp"
#include "MemoryManagement/MyAllocator.hpp"

#include <vector>
#include <list>
#include <map>

using namespace testing;
using namespace mybicycles;

TEST(BicyclesCustomAllocatorTestSuite, Allocator_CompilationTest)
{
    // Make sure that our Allocator implementation corresponds to STL containers' requirements
    // (and so they compile together):
    std::vector<int, MyAllocator<int>> vec1;
    std::vector<int, MyAllocator<int, DummySegmentManager>> vec2;

    // Make sure our Allocator can receive to-be-managed memory segment as a parameter:
    const size_t segSize = 128;
    char segment[segSize];
    SharedPtr<SimpleSegmentManager> ssm = makeShared<SimpleSegmentManager>(segment, segSize);
    MyAllocator<int, SimpleSegmentManager> myal(ssm);
    std::vector<int, MyAllocator<int, SimpleSegmentManager>> vec3(myal);
}

TEST(BicyclesCustomAllocatorTestSuite, Allocator_ContainerObjects)
{
    // Test allocator with different containers with memory pre-allocated on heap.
    // Make sure all the objects are properly constructed & destructed as containers expand.
    // Test the case when multiple containers of different types use the same Allocator.
    const size_t segSize = 4096; // 4KB
    char* segment = (char*)malloc(segSize);
    SharedPtr<SimpleSegmentManager> ssm = makeShared<SimpleSegmentManager>(segment,
                                                                           segSize,
                                                                           true,
                                                                           [](char* segment,size_t){ free(segment); });
    MyAllocator<BicycleImpl, SimpleSegmentManager> myal(ssm, true);

    std::cout << "Sizeof BicycleImpl: " << sizeof(BicycleImpl) << std::endl;
    const size_t elNum = 32;

    std::cout << "=======================std::vector=======================" << std::endl;
    std::vector<BicycleImpl, MyAllocator<BicycleImpl, SimpleSegmentManager>> vec(myal);
    for (int i = 0; i < elNum; i++)
    {
        vec.push_back({"Bicycle"});
    }
    EXPECT_EQ(vec.size(), elNum);
    vec.clear();
    vec.shrink_to_fit();

    std::cout << "=======================std::list=========================" << std::endl;
    std::list<BicycleImpl, MyAllocator<std::_List_node<mybicycles::BicycleImpl>, SimpleSegmentManager>> l(
                MyAllocator<std::_List_node<mybicycles::BicycleImpl>, SimpleSegmentManager>(ssm, true));
    l.push_back({"Bicycle"});



}

//TEST(BicyclesCustomAllocatorTestSuite, Allocator_NegativeCase)
//{
//    // TODO: Expect BadAlloc be thrown when Allocator runs out of pre-allocated memory
//}

//TEST(BicyclesCustomAllocatorTestSuite, Allocator_MultiThreadingScenario)
//{
//    // TODO: launch multiple threads competing for memory resource
//}
