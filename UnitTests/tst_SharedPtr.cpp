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
        EXPECT_TRUE(sp1 == SharedPtr<MockBicycle>());
        EXPECT_FALSE(sp2 == SharedPtr<MockBicycle>());

        EXPECT_FALSE(sp1 != nullptr);
        EXPECT_FALSE(sp1 != SharedPtr<MockBicycle>());
        EXPECT_TRUE(sp2 != SharedPtr<MockBicycle>());

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

TEST(BicyclesSharedPtrTestSuite, SharedPtr_OtherComparisons)
{
    auto fakeDeleter = [](MockBicycle* ptr){ /* Deletes nothing, haha */ };

    SharedPtr<MockBicycle> sp1(reinterpret_cast<MockBicycle*>(14), fakeDeleter);
    SharedPtr<MockBicycle> sp2(reinterpret_cast<MockBicycle*>(17), fakeDeleter);
    SharedPtr<MockBicycle> sp3 = sp2;

    EXPECT_TRUE(sp1 < sp2);
    EXPECT_FALSE(sp2 < sp1);
    EXPECT_FALSE(sp2 < SharedPtr<MockBicycle>());

    EXPECT_TRUE(sp2 > sp1);
    EXPECT_FALSE(sp1 > sp2);
    EXPECT_TRUE(sp2 > SharedPtr<MockBicycle>());

    EXPECT_TRUE(sp1 <= sp2);
    EXPECT_TRUE(sp1 <= sp1);
    EXPECT_FALSE(sp2 <= sp1);
    EXPECT_TRUE(sp2 <= sp3);
    EXPECT_FALSE(sp2 <= SharedPtr<MockBicycle>());

    EXPECT_TRUE(sp2 >= sp1);
    EXPECT_TRUE(sp2 >= sp2);
    EXPECT_FALSE(sp1 >= sp2);
    EXPECT_TRUE(sp2 >= sp3);
    EXPECT_TRUE(sp2 >= SharedPtr<MockBicycle>());
}

void deleterFunc(MockBicycle* ptr)
{
    delete ptr;
}

struct DeleterFunctor
{
    void operator()(MockBicycle* ptr)
    {
        delete ptr;
    }
};

TEST(BicyclesSharedPtrTestSuite, SharedPtr_CustomDeleter)
{
    MockBicycle* mb1 = new MockBicycle("Giant’s Revolt");
    MockBicycle* mb2 = new MockBicycle("Canyon");
    MockBicycle* mb3 = new MockBicycle[3]{{"Ritte"}, {"Nishiki"}, {"Fargo"}};
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    EXPECT_CALL(mb3[1], die());
    EXPECT_CALL(mb3[2], die());
    EXPECT_CALL(mb3[0], die());

    {
        // Custom deleter as a func pointer
        SharedPtr<MockBicycle> sp1(mb1, deleterFunc);

        // Custom deleter as a functor
        SharedPtr<MockBicycle> sp2(mb2, DeleterFunctor());

        // Custom deleter as a lambda
        SharedPtr<MockBicycle> sp3(mb3, [](MockBicycle* ptr){delete[] ptr;});
    }
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_MakeShared)
{
    {
        SharedPtr<MockBicycle> sp1 = makeShared<MockBicycle>("Trek's Émonda", 60, 59);
        SharedPtr<MockBicycle> sp2 = sp1;
        EXPECT_CALL(*sp1.get(), die());
        EXPECT_EQ(sp1.get(), sp2.get());
        EXPECT_EQ(sp1.useCount(), 2);
        EXPECT_EQ(sp2.useCount(), 2);
    }
}

class SharedEnabledMockBicycle : public MockBicycle, public EnableSharedFromThis<SharedEnabledMockBicycle>
{
public:
    SharedEnabledMockBicycle(std::string vendor) :
        MockBicycle(vendor),
        EnableSharedFromThis<SharedEnabledMockBicycle>()
    {}
};

TEST(BicyclesSharedPtrTestSuite, SharedPtr_EnableSharedFromThis_CorrectUse)
{
    // Two SharedPtr-s share the same object
    SharedPtr<SharedEnabledMockBicycle> sp1 = makeShared<SharedEnabledMockBicycle>("Giant");
    SharedPtr<SharedEnabledMockBicycle> sp2 = sp1->getSharedFromThis();
    EXPECT_EQ(sp1.useCount(), 2);
    EXPECT_EQ(sp2.useCount(), 2);

    WeakPtr<SharedEnabledMockBicycle> wp = sp2->getWeakFromThis();
    EXPECT_EQ(wp.useCount(), 2);
    EXPECT_FALSE(wp.isExpired());

    SharedPtr<SharedEnabledMockBicycle> sp3 = wp.lock();
    EXPECT_EQ(sp1.useCount(), 3);
    EXPECT_EQ(sp2.useCount(), 3);
    EXPECT_EQ(sp3.useCount(), 3);

    EXPECT_EQ(sp1, sp2);
    EXPECT_EQ(sp2, sp3);

    EXPECT_CALL(*sp1, die());
}

TEST(BicyclesSharedPtrTestSuite, SharedPtr_EnableSharedFromThis_Misuse)
{
    SharedEnabledMockBicycle* mb = new SharedEnabledMockBicycle("Scott");
    EXPECT_CALL(*mb, die());

    {
        WeakPtr<SharedEnabledMockBicycle> wp = mb->getWeakFromThis();
        EXPECT_TRUE(wp.isExpired());
        EXPECT_EQ(wp.useCount(), 0);

        // getSharedFromThis is called without SharedPtr owning the caller
        EXPECT_THROW(mb->getSharedFromThis(), BadWeakPtr);

        // mb starts being owned, no BadWeakPtr exception expected now
        SharedPtr<SharedEnabledMockBicycle> sp(mb);
        EXPECT_NO_THROW(wp = mb->getSharedFromThis()); // update wp
        EXPECT_FALSE(wp.isExpired());
        EXPECT_EQ(wp.useCount(), 1);
        EXPECT_EQ(sp.useCount(), 1);
    }
}
