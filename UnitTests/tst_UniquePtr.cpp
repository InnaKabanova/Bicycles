#include <string>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "MockBicycle.hpp"
#include "UniquePtr.hpp"

using namespace testing;
using namespace mybicycles;

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Construction_Destruction)
{
    UniquePtr<MockBicycle> up1;
    EXPECT_EQ(up1.get(), nullptr);

    MockBicycle* mb = new MockBicycle("Trek");
    EXPECT_CALL(*mb, die());
    {
        UniquePtr<MockBicycle> up2(mb);
        EXPECT_EQ(up2.get(), mb);
    }
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_MoveConstruction)
{
    const std::string VENDOR = "Raleigh";
    const int PRESSURE_FRONT = 51;
    const int PRESSURE_REAR = 52;

    MockBicycle* mb = new MockBicycle(VENDOR, PRESSURE_FRONT, PRESSURE_REAR);
    EXPECT_CALL(*mb, die());
    {
        UniquePtr<MockBicycle> up1(mb);
        EXPECT_EQ(up1.get(), mb);

        UniquePtr<MockBicycle> up2(std::move(up1));
        EXPECT_EQ(up1.get(), nullptr);
        EXPECT_EQ(up2.get(), mb);
        EXPECT_EQ(up2->getVendor(), VENDOR);
        EXPECT_EQ(up2->getPressureFront(), PRESSURE_FRONT);
        EXPECT_EQ(up2->getPressureRear(), PRESSURE_REAR);
    }
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_MoveAssignment)
{
    const std::string VENDOR = "Moxi";
    const int PRESSURE_FRONT = 53;
    const int PRESSURE_REAR = 54;

    MockBicycle* mb1 = new MockBicycle(VENDOR, PRESSURE_FRONT, PRESSURE_REAR);
    MockBicycle* mb2 = new MockBicycle("Giant");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    {
        UniquePtr<MockBicycle> up1(mb1);
        UniquePtr<MockBicycle> up2(mb2);
        up2 = std::move(up1);

        EXPECT_EQ(up1.get(), nullptr);
        EXPECT_EQ(up2.get(), mb1);
        EXPECT_EQ(up2->getVendor(), VENDOR);
        EXPECT_EQ(up2->getPressureFront(), PRESSURE_FRONT);
        EXPECT_EQ(up2->getPressureRear(), PRESSURE_REAR);
    }
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Release)
{
    MockBicycle* mb1 = new MockBicycle("Scott");
    MockBicycle* mb2 = nullptr;

    {
        UniquePtr<MockBicycle> up(mb1);
        mb2 = up.release();
        EXPECT_EQ(up.get(), nullptr);
    }
    EXPECT_EQ(mb1, mb2);

    delete mb1;
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Reset)
{
    MockBicycle* mb1 = new MockBicycle("Fuji");
    MockBicycle* mb2 = new MockBicycle("Santa Cruz");
    MockBicycle* mb3 = new MockBicycle("Colnago");
    EXPECT_CALL(*mb1, die());
    EXPECT_CALL(*mb2, die());
    EXPECT_CALL(*mb3, die());
    {
        UniquePtr<MockBicycle> up1(mb1);
        up1.reset();
        EXPECT_EQ(up1.get(), nullptr);

        up1 = UniquePtr<MockBicycle>(mb2);
        up1.reset(mb3);
    }
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Swap_BoolOperators)
{
    std::string VENDOR_A = "Bianchi";
    int8_t PRESSURE_FRONT_A = 49;
    int8_t PRESSURE_REAR_A = 48;

    std::string VENDOR_B = "Cannondale";
    int8_t PRESSURE_FRONT_B = 60;
    int8_t PRESSURE_REAR_B = 61;

    {
        MockBicycle* mb1 = new MockBicycle(VENDOR_A, PRESSURE_FRONT_A, PRESSURE_REAR_A);
        UniquePtr<MockBicycle> up1(mb1);
        MockBicycle* mb2 = new MockBicycle(VENDOR_B, PRESSURE_FRONT_B, PRESSURE_REAR_B);
        UniquePtr<MockBicycle> up2(mb2);

        UniquePtr<MockBicycle>::swap(up1, up2);

        EXPECT_EQ(up1.get(), mb2);
        EXPECT_EQ(up2.get(), mb1);
        EXPECT_EQ(up1->getVendor(), VENDOR_B);
        EXPECT_EQ(up2->getVendor(), VENDOR_A);
        EXPECT_EQ(up1->getPressureFront(), PRESSURE_FRONT_B);
        EXPECT_EQ(up1->getPressureRear(), PRESSURE_REAR_B);
        EXPECT_EQ(up2->getPressureFront(), PRESSURE_FRONT_A);
        EXPECT_EQ(up2->getPressureRear(), PRESSURE_REAR_A);

        up1.reset();
        EXPECT_TRUE(up1 == nullptr);
        EXPECT_FALSE(up1 != nullptr);
        EXPECT_TRUE(up2);
        EXPECT_FALSE(up1);
    }
}
