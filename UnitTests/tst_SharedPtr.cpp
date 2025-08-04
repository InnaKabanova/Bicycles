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
    MockBicycle* mb3 = new MockBicycle("Civia");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    EXPECT_CALL(*mb3, die());
    {
        // Check self-assignment:
        SharedPtr<MockBicycle> sp1(mb1);
        sp1 = sp1;
        EXPECT_EQ(sp1.get(), mb1);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);

        // Check assignment of empty rhs:
        SharedPtr<MockBicycle> empty;
        EXPECT_EQ(empty.get(), nullptr);
        EXPECT_EQ(empty.isUnique(), true);
        EXPECT_EQ(empty.useCount(), 0);
        sp1 = empty;
        EXPECT_EQ(sp1.get(), nullptr);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 0);

        // Check assignment of non-empty rhs to empty sp:
        SharedPtr<MockBicycle> sp2(mb2);
        sp1 = sp2;
        EXPECT_EQ(sp1.get(), mb2);
        EXPECT_EQ(sp1.isUnique(), false);
        EXPECT_EQ(sp1.useCount(), 2);

        // Check assignment of non-empty rhs to non-empty sp:
        SharedPtr<MockBicycle> sp3(mb3);
        sp2 = sp3;
        EXPECT_EQ(sp2.get(), mb3);
        EXPECT_EQ(sp2.isUnique(), false);
        EXPECT_EQ(sp2.useCount(), 2);
        EXPECT_EQ(sp1.get(), mb2);
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
    MockBicycle* mb = new MockBicycle("Niner Bikes");
    EXPECT_CALL(*mb, die());
    {
        SharedPtr<MockBicycle> sp1(mb);
        EXPECT_EQ(sp1.get(), mb);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 1);

        SharedPtr<MockBicycle> sp2(sp1);
        EXPECT_EQ(sp2.get(), mb);
        EXPECT_EQ(sp2.isUnique(), false);
        EXPECT_EQ(sp2.useCount(), 2);

        SharedPtr<MockBicycle> sp3;
        sp3 = std::move(sp1);
        EXPECT_EQ(sp1.get(), nullptr);
        EXPECT_EQ(sp1.isUnique(), true);
        EXPECT_EQ(sp1.useCount(), 0);
        EXPECT_EQ(sp3.get(), mb);
        EXPECT_EQ(sp3.isUnique(), false);
        EXPECT_EQ(sp3.useCount(), 2);
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_Reset)
{
    MockBicycle* mb1 = new MockBicycle("Merida");
    MockBicycle* mb2 = new MockBicycle("Diamondback");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    {
        // Reset empty sp:
        SharedPtr<MockBicycle> sp;
        sp.reset();
        EXPECT_EQ(sp.get(), nullptr);
        EXPECT_EQ(sp.isUnique(), true);
        EXPECT_EQ(sp.useCount(), 0);

        // Reset empty sp with non-null ptr:
        sp.reset(mb1);
        EXPECT_EQ(sp.get(), mb1);
        EXPECT_EQ(sp.isUnique(), true);
        EXPECT_EQ(sp.useCount(), 1);

        // Reset non-empty sp with another non-null ptr:
        sp.reset(mb2);
        EXPECT_EQ(sp.get(), mb2);
        EXPECT_EQ(sp.isUnique(), true);
        EXPECT_EQ(sp.useCount(), 1);

        // Reset non-empty sp:
        sp.reset();
        EXPECT_EQ(sp.get(), nullptr);
        EXPECT_EQ(sp.isUnique(), true);
        EXPECT_EQ(sp.useCount(), 0);
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

TEST(BicyclesSharedPtrTestSuite, SharedPtr_Equality_NonEquality)
{
    MockBicycle* mb1 = new MockBicycle("Pinarello");
    SharedPtr<MockBicycle> sp1;
    EXPECT_TRUE(sp1 == nullptr);
    EXPECT_FALSE(sp1 != nullptr);

    sp1.reset(mb1);
    EXPECT_TRUE(sp1 != nullptr);
    EXPECT_FALSE(sp1 == nullptr);
    EXPECT_EQ(sp1, sp1);

    SharedPtr<MockBicycle> sp2 = sp1;
    EXPECT_EQ(sp1, sp2);

    SharedPtr<MockBicycle> sp3(new MockBicycle("Yeti"));
    EXPECT_NE(sp2, sp3);
}

template <typename T>
class FakeDeleter
{
public:
    static void deletePtr(T* ptr) noexcept
    {
        // Deletes nothing, haha
    }
};

TEST(BicyclesSharedPtrTestSuite, SharedPtr_OtherComparisons)
{
    SharedPtr<MockBicycle, FakeDeleter<MockBicycle>> sp1(reinterpret_cast<MockBicycle*>(14));
    SharedPtr<MockBicycle, FakeDeleter<MockBicycle>> sp2(reinterpret_cast<MockBicycle*>(17));
    SharedPtr<MockBicycle, FakeDeleter<MockBicycle>> sp3 = sp2;

    EXPECT_TRUE(sp1 < sp2);
    EXPECT_FALSE(sp2 < sp1);

    EXPECT_TRUE(sp2 > sp1);
    EXPECT_FALSE(sp1 > sp2);

    EXPECT_TRUE(sp1 <= sp2);
    EXPECT_TRUE(sp1 <= sp1);
    EXPECT_FALSE(sp2 <= sp1);
    EXPECT_TRUE(sp2 <= sp3);

    EXPECT_TRUE(sp2 >= sp1);
    EXPECT_TRUE(sp2 >= sp2);
    EXPECT_FALSE(sp1 >= sp2);
    EXPECT_TRUE(sp2 >= sp3);
}

//--------------------------------------------------------------------------------------------------

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
