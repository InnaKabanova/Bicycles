#include <gtest/gtest.h>

#include "MockBicycle.hpp"
#include "SharedPtr.hpp"

using namespace testing;
using namespace mybicycles;

TEST(BicyclesSharedPtrTestSuite, WeakPtr_Construction_Destruction_UseCount)
{
    MockBicycle* mb = new MockBicycle("Dahon");
    EXPECT_CALL(*mb, die());

    WeakPtr<MockBicycle> wp1;
    EXPECT_EQ(wp1.useCount(), 0);
    EXPECT_TRUE(wp1.isExpired());

    {
        SharedPtr<MockBicycle> sp1(mb);
        WeakPtr<MockBicycle> wp2(sp1);
        EXPECT_EQ(wp2.useCount(), 1);
        EXPECT_FALSE(wp2.isExpired());

        SharedPtr<MockBicycle> sp2 = sp1;
        EXPECT_EQ(wp2.useCount(), 2);
        EXPECT_FALSE(wp2.isExpired());

        sp1.reset();
        sp2.reset();
        EXPECT_TRUE(wp2.isExpired());
    }
}

TEST(BicyclesSharedPtrTestSuite, WeakPtr_CopyConstrution_Lock)
{
    MockBicycle* mb = new MockBicycle("All-City");
    EXPECT_CALL(*mb, die());

    {
        SharedPtr<MockBicycle> sp1(mb);
        WeakPtr<MockBicycle> wp1(sp1);
        EXPECT_EQ(wp1.useCount(), 1);
        EXPECT_FALSE(wp1.isExpired());

        SharedPtr<MockBicycle> sp2 = wp1.lock();
        WeakPtr<MockBicycle> wp2(wp1);
        SharedPtr<MockBicycle> sp3 = wp2.lock();
        EXPECT_EQ(sp1.get(), sp2.get());
        EXPECT_EQ(sp1.get(), sp3.get());
        EXPECT_EQ(wp1.useCount(), 3);
        EXPECT_FALSE(wp1.isExpired());

        sp1.reset();
        sp2.reset();
        sp3.reset();
        EXPECT_EQ(wp1.useCount(), 0);
        EXPECT_TRUE(wp1.isExpired());
        EXPECT_EQ(wp2.useCount(), 0);
        EXPECT_TRUE(wp2.isExpired());
    }
}

TEST(BicyclesSharedPtrTestSuite, WeakPtr_CopyAssignment_Lock)
{
    MockBicycle* mb1 = new MockBicycle("Breezer");
    MockBicycle* mb2 = new MockBicycle("Trek’s Electra");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());

    {
        SharedPtr<MockBicycle> sp1(mb1);
        WeakPtr<MockBicycle> wp1(sp1);
        EXPECT_EQ(wp1.useCount(), 1);
        EXPECT_FALSE(wp1.isExpired());

        SharedPtr<MockBicycle> sp2 = wp1.lock();
        EXPECT_EQ(sp2.get(), mb1);
        EXPECT_EQ(wp1.useCount(), 2);
        EXPECT_FALSE(wp1.isExpired());

        SharedPtr<MockBicycle> sp3(mb2);
        WeakPtr<MockBicycle> wp2(sp3);
        wp1 = wp2;
        EXPECT_EQ(wp1.useCount(), 1);
        EXPECT_FALSE(wp1.isExpired());
        SharedPtr<MockBicycle> sp4 = wp1.lock();
        EXPECT_EQ(sp4.get(), mb2);
    }
}

TEST(BicyclesSharedPtrTestSuite, WeakPtr_MoveConstrution)
{
    MockBicycle* mb = new MockBicycle("Salsa Cycles");
    EXPECT_CALL(*mb, die());

    {
        SharedPtr<MockBicycle> sp(mb);
        WeakPtr<MockBicycle> wp1(sp);
        EXPECT_EQ(wp1.useCount(), 1);
        EXPECT_FALSE(wp1.isExpired());

        WeakPtr<MockBicycle> wp2(std::move(wp1));
        EXPECT_EQ(wp1.useCount(), 0);
        EXPECT_TRUE(wp1.isExpired());
        EXPECT_EQ(wp2.useCount(), 1);
        EXPECT_FALSE(wp2.isExpired());
    }
}

TEST(BicyclesSharedPtrTestSuite, WeakPtr_MoveAssignment)
{
    MockBicycle* mb1 = new MockBicycle("Surly");
    MockBicycle* mb2 = new MockBicycle("Marin Bikes");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());

    {
        SharedPtr<MockBicycle> sp1(mb1);
        WeakPtr<MockBicycle> wp1(sp1);
        EXPECT_EQ(wp1.useCount(), 1);
        EXPECT_FALSE(wp1.isExpired());

        SharedPtr<MockBicycle> sp2(mb2);
        WeakPtr<MockBicycle> wp2(sp2);
        wp2 = std::move(wp1);
        EXPECT_EQ(wp1.useCount(), 0);
        EXPECT_TRUE(wp1.isExpired());
        EXPECT_EQ(wp2.useCount(), 1);
        EXPECT_FALSE(wp2.isExpired());
    }
}

TEST(BicyclesSharedPtrTestSuite, WeakPtr_Reset_Expired)
{
    MockBicycle* mb = new MockBicycle("Felt Bicycles");
    EXPECT_CALL(*mb, die());

    WeakPtr<MockBicycle> wp1;
    WeakPtr<MockBicycle> wp2;
    {
        SharedPtr<MockBicycle> sp(mb);
        WeakPtr<MockBicycle> wp3(sp);

        wp1 = wp3;
        EXPECT_EQ(wp1.useCount(), 1);
        EXPECT_FALSE(wp1.isExpired());
        wp1.reset();
        EXPECT_EQ(wp1.useCount(), 0);
        EXPECT_TRUE(wp1.isExpired());

        wp2 = wp3;
    }

    EXPECT_EQ(wp2.useCount(), 0);
    EXPECT_TRUE(wp2.isExpired());
}

TEST(BicyclesSharedPtrTestSuite, WeakPtr_Swap)
{
    MockBicycle* mb1 = new MockBicycle("Masi Bikes");
    MockBicycle* mb2 = new MockBicycle("Fargo");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());

    WeakPtr<MockBicycle> wp1;
    WeakPtr<MockBicycle> wp2;
    {
        SharedPtr<MockBicycle> sp1(mb1);
        SharedPtr<MockBicycle> sp2(mb2);
        wp1 = sp1;
        wp2 = sp2;

        WeakPtr<MockBicycle>::swap(wp1, wp2);
        EXPECT_EQ(wp1.lock().get(), sp2.get());
        EXPECT_FALSE(wp1.isExpired());
        EXPECT_EQ(wp2.lock().get(), sp1.get());
        EXPECT_FALSE(wp2.isExpired());
    }

    EXPECT_EQ(wp1.lock(), nullptr);
    EXPECT_TRUE(wp1.isExpired());
    EXPECT_EQ(wp2.lock(), nullptr);
    EXPECT_TRUE(wp2.isExpired());
}
