#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "SharedPtr.hpp"

using namespace testing;
using namespace mybicycles;

TEST(BicyclesSharedPtrTestSuite, SharedPtr_Construction_Destruction)
{
    SharedPtr<int> sp;
    int numDestructorsCalled = 0;
    EXPECT_EQ(numDestructorsCalled, 0);
}

