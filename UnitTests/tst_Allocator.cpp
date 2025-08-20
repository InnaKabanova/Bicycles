#include <gtest/gtest.h>

// #include "MockBicycle.hpp"
#include "Examples/Allocator_Example.hpp"

using namespace testing;


TEST(BicyclesCustomAllocatorTestSuite, Allocator_ContainerObjectsOnHeap)
{
    // TODO: test allocator with different containers with memory pre-allocated on heap
    //       make sure all the objects are properly constructed & destructed as containers expand

    // TODO: test the case when multiple containers of different types use the same Allocator

    testMySimpleAllocator();
    EXPECT_EQ(1, 1);
}

TEST(BicyclesCustomAllocatorTestSuite, Allocator_ContainerObjectsOnStack)
{
    // TODO: test allocator with different containers with memory pre-allocated on stack
    //       make sure all the objects are properly constructed & destructed as containers expand
}

TEST(BicyclesCustomAllocatorTestSuite, Allocator_NegativeCase)
{
    // TODO: Expect BadAlloc be thrown when Allocator runs out of pre-allocated memory
}

TEST(BicyclesCustomAllocatorTestSuite, Allocator_MultiThreadingScenario)
{
    // TODO: launch multiple threads competing for memory resource
}
