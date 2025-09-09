#include <gtest/gtest.h>

#include "BicycleImpl.hpp"
#include "MemoryManagement/MyAllocatorOnStack.hpp"
#include "MemoryManagement/MyAllocatorNonOwning.hpp"

#include <vector>
#include <list>
#include <map>
#include <set>

using namespace testing;
using namespace mybicycles;

TEST(BicyclesCustomAllocatorsTestSuite, BasicCompilationTest)
{
    // Make sure that our Allocator implementation corresponds to STL containers' requirements
    // (and so they compile together):
    std::vector<int, MyAllocatorOnStack<int, 128>> vec1;
    std::vector<int, MyAllocatorOnStack<int, 128, DummySegmentManager>> vec2;

    // Submit a to-be-managed memory segment as a parameter to MyAllocatorNonOwning:
    const size_t segSize = 128;
    char seg[segSize];
    SharedPtr<SimpleSegmentManager> ssm = makeShared<SimpleSegmentManager>(seg, segSize);
    MyAllocatorNonOwning<int, SimpleSegmentManager> myal(ssm);
    std::vector<int, MyAllocatorNonOwning<int, SimpleSegmentManager>> vec3(myal);
}

TEST(BicyclesCustomAllocatorsTestSuite, BasicNegativeCase)
{
    // Simulate an out-of-memory scenario
    constexpr size_t SEG_SIZE = sizeof(BicycleImpl) - 1;
    MyAllocatorOnStack<int, SEG_SIZE> myal;
    std::vector<BicycleImpl, MyAllocatorOnStack<int, SEG_SIZE>> vec(myal);
    EXPECT_THROW(vec.push_back({"BicycleX"}), std::runtime_error);
}

template<typename MyBicyclesAllocator, typename MyBicyclesPairAllocator>
void testAllocatorWithContainers(MyBicyclesAllocator myal)
{
    const size_t num = 16;

    {
        std::cout << "=======================std::vector=======================" << std::endl;
        std::vector<BicycleImpl, MyBicyclesAllocator> vec(myal);

        for (int i = 0; i < num; i++)
        {
            vec.push_back({"Bicycle-V-" + std::to_string(i)});
        }
        EXPECT_EQ(vec.size(), num);

        for (int i = 0; i < num; i++)
        {
            EXPECT_EQ(vec[i].getVendor(), "Bicycle-V-" + std::to_string(i));
        }

        vec.clear();
        EXPECT_EQ(vec.size(), 0);
        vec.shrink_to_fit();
        EXPECT_EQ(vec.capacity(), 0);
    }

    {
        std::cout << "=======================std::list=========================" << std::endl;
        std::list<BicycleImpl, MyBicyclesAllocator> lst(myal);

        for (int i = 0; i < num; i++)
        {
            lst.push_back({"Bicycle-L-" + std::to_string(i)});
        }
        EXPECT_EQ(lst.size(), num);

        int index = 0;
        for (const auto& l : lst)
        {
            EXPECT_EQ(l.getVendor(), "Bicycle-L-" + std::to_string(index++));
        }
        lst.clear();
        EXPECT_EQ(lst.size(), 0);
    }

    {
        std::cout << "=======================std::map===========================" << std::endl;
        std::map<int, BicycleImpl, std::less<int>, MyBicyclesPairAllocator> mp(myal);

        for (int i = 0; i < num; i++)
        {
            mp.emplace(i, BicycleImpl("Bicycle-M-" + std::to_string(i)));
        }

        for (int i = 0; i < num; i++)
        {
            EXPECT_EQ(mp.at(i).getVendor(), "Bicycle-M-" + std::to_string(i));
        }
        mp.clear();
        EXPECT_EQ(mp.size(), 0);
    }

    {
        std::cout << "=======================std::set===========================" << std::endl;
        std::set<BicycleImpl, std::less<BicycleImpl>, MyBicyclesAllocator> st(myal);

        for (int i = 0; i < num; i++)
        {
            st.emplace("Bicycle-S-" + std::to_string(i));
        }
        EXPECT_EQ(st.size(), num);

        int index = 0;
        for (const auto& s : st)
        {
            EXPECT_EQ(s.getVendor(), "Bicycle-S-" + std::to_string(index++));
        }
        st.clear();
        EXPECT_EQ(st.size(), 0);
    }
}

TEST(BicyclesCustomAllocatorsTestSuite, MyAllocatorOnStack_ContainerObjects)
{
    constexpr bool ALLOC_LOGGING = true;
    constexpr size_t SEG_SIZE = 5120; // 5KB

    using MyBicyclesAllocatorOnStack = MyAllocatorOnStack<BicycleImpl, SEG_SIZE, SimpleSegmentManager>;
    using MyBicyclesPairAllocatorOnStack = MyAllocatorOnStack<std::pair<const int, BicycleImpl>, SEG_SIZE, SimpleSegmentManager>;

    MyBicyclesAllocatorOnStack myal(ALLOC_LOGGING);
    testAllocatorWithContainers<MyBicyclesAllocatorOnStack, MyBicyclesPairAllocatorOnStack>(myal);
}

TEST(BicyclesCustomAllocatorsTestSuite, MyAllocatorNonOwning_ContainerObjects)
{
    constexpr bool ALLOC_LOGGING = true;
    constexpr size_t SEG_SIZE = 5120; // 5KB

    using MyBicyclesAllocatorNonOwning = MyAllocatorNonOwning<BicycleImpl, SimpleSegmentManager>;
    using MyBicyclesPairAllocatorNonOwning = MyAllocatorNonOwning<std::pair<const int, BicycleImpl>, SimpleSegmentManager>;

    char* seg = (char*)malloc(SEG_SIZE);
    SharedPtr<SimpleSegmentManager> ssm = makeShared<SimpleSegmentManager>(seg, SEG_SIZE);
    MyBicyclesAllocatorNonOwning myal(ssm, ALLOC_LOGGING);
    testAllocatorWithContainers<MyBicyclesAllocatorNonOwning, MyBicyclesPairAllocatorNonOwning>(myal);
    free(seg);
}
