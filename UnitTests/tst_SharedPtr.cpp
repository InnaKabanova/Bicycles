// #include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "MockBicycle.hpp"
#include "SharedPtr.hpp"

using namespace testing;
using namespace mybicycles;

TEST(BicyclesSharedPtrTestSuite, SharedPtr_Construction_Destruction)
{
    SharedPtr<MockBicycle> sp1;
    EXPECT_EQ(sp1.get(), nullptr);
    EXPECT_EQ(sp1.isUnique(), true);
    EXPECT_EQ(sp1.useCount(), 0);

    MockBicycle* mb = new MockBicycle("Fuji");
    EXPECT_CALL(*mb, die());
    {
        SharedPtr<MockBicycle> sp2(mb);
        sp1 = sp2;
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_CopyConstruction)
{
    MockBicycle* mb = new MockBicycle("Giant");
    EXPECT_CALL(*mb, die());
    {
        SharedPtr<MockBicycle> sp1(mb);
        EXPECT_EQ(sp1.get(), mb);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);

        SharedPtr<MockBicycle> sp2(sp1);
        SharedPtr<MockBicycle> sp3(sp1);
        SharedPtr<MockBicycle> sp4(sp2);

        EXPECT_EQ(sp1.get(), mb);
        EXPECT_EQ(sp1.isUnique(), false);
        EXPECT_EQ(sp1.useCount(), 4);
        EXPECT_EQ(sp2.get(), mb);
        EXPECT_EQ(sp2.isUnique(), false);
        EXPECT_EQ(sp2.useCount(), 4);
        EXPECT_EQ(sp3.get(), mb);
        EXPECT_EQ(sp3.isUnique(), false);
        EXPECT_EQ(sp3.useCount(), 4);
        EXPECT_EQ(sp4.get(), mb);
        EXPECT_EQ(sp4.isUnique(), false);
        EXPECT_EQ(sp4.useCount(), 4);
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_CopyAssignment)
{
    MockBicycle* mb1 = new MockBicycle("Trek");
    MockBicycle* mb2 = new MockBicycle("Colnago");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    {
        SharedPtr<MockBicycle> sp1(mb1);
        sp1 = sp1; // check self-assignment
        EXPECT_EQ(sp1.get(), mb1);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);

        SharedPtr<MockBicycle> sp2;
        EXPECT_EQ(sp2.get(), nullptr);
        EXPECT_EQ(sp2.isUnique(), true);
        EXPECT_EQ(sp2.useCount(), 0);

        sp2 = sp1;
        EXPECT_EQ(sp2.get(), mb1);
        EXPECT_EQ(sp2.isUnique(), false);
        EXPECT_EQ(sp2.useCount(), 2);

        SharedPtr<MockBicycle> sp3(mb2);
        sp2 = sp3;
        EXPECT_EQ(sp2.get(), mb2);
        EXPECT_EQ(sp2.isUnique(), false);
        EXPECT_EQ(sp2.useCount(), 2);
        EXPECT_EQ(sp1.get(), mb1);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_MoveConstruction)
{
    MockBicycle* mb = new MockBicycle("Bianchi");
    EXPECT_CALL(*mb, die());
    {
        SharedPtr<MockBicycle> sp1(mb);
        EXPECT_EQ(sp1.get(), mb);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);

        SharedPtr<MockBicycle> sp2(std::move(sp1));
        EXPECT_EQ(sp1.get(), nullptr);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 0);
        EXPECT_EQ(sp2.get(), mb);
        EXPECT_EQ(sp2.isUnique(), true);
        EXPECT_EQ(sp2.useCount(), 1);
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_MoveAssignment)
{
    MockBicycle* mb = new MockBicycle("Bianchi");
    EXPECT_CALL(*mb, die());
    {
        SharedPtr<MockBicycle> sp1(mb);
        EXPECT_EQ(sp1.get(), mb);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);

        SharedPtr<MockBicycle> sp2;
        sp2 = std::move(sp1);
        EXPECT_EQ(sp1.get(), nullptr);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 0);
        EXPECT_EQ(sp2.get(), mb);
        EXPECT_EQ(sp2.isUnique(), true);
        EXPECT_EQ(sp2.useCount(), 1);
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_Reset)
{
    MockBicycle* mb1 = new MockBicycle("Merida");
    MockBicycle* mb2 = new MockBicycle("Diamondback");
    MockBicycle* mb3 = new MockBicycle("Norco");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    EXPECT_CALL(*mb3, die());
    {
        SharedPtr<MockBicycle> sp1(mb1);
        sp1.reset();
        EXPECT_EQ(sp1.get(), nullptr);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 0);

        SharedPtr<MockBicycle> sp2(mb2);
        sp2.reset(mb3);
        EXPECT_EQ(sp2.get(), mb3);
        EXPECT_EQ(sp2.isUnique(), true);
        EXPECT_EQ(sp2.useCount(), 1);
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_Swap_BoolOperators)
{
    std::string VENDOR_A = "Scott";
    int8_t PRESSURE_FRONT_A = 49;
    int8_t PRESSURE_REAR_A = 48;

    std::string VENDOR_B = "Specialized";
    int8_t PRESSURE_FRONT_B = 60;
    int8_t PRESSURE_REAR_B = 61;

    {
        MockBicycle* mb1 = new MockBicycle(VENDOR_A, PRESSURE_FRONT_A, PRESSURE_REAR_A);
        SharedPtr<MockBicycle> sp1(mb1);
        MockBicycle* mb2 = new MockBicycle(VENDOR_B, PRESSURE_FRONT_B, PRESSURE_REAR_B);
        SharedPtr<MockBicycle> sp2(mb2);

        SharedPtr<MockBicycle>::swap(sp1, sp2);

        EXPECT_EQ(sp1.get(), mb2);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);
        EXPECT_EQ(sp2.get(), mb1);
        EXPECT_EQ(sp2.isUnique(), true);
        EXPECT_EQ(sp2.useCount(), 1);
        EXPECT_EQ(sp1->getVendor(), VENDOR_B);
        EXPECT_EQ(sp2->getVendor(), VENDOR_A);
        EXPECT_EQ(sp1->getPressureFront(), PRESSURE_FRONT_B);
        EXPECT_EQ(sp1->getPressureRear(), PRESSURE_REAR_B);
        EXPECT_EQ(sp2->getPressureFront(), PRESSURE_FRONT_A);
        EXPECT_EQ(sp2->getPressureRear(), PRESSURE_REAR_A);

        sp1.reset();
        EXPECT_TRUE(sp1 == nullptr);
        EXPECT_FALSE(sp1 != nullptr);
        EXPECT_TRUE(sp2);
        EXPECT_FALSE(sp1);
    }
}
