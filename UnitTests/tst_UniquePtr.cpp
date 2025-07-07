#include <string>
#include <functional>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "UniquePtr.hpp"

using namespace testing;
using namespace mybicycles;

struct RollerSkates
{
    RollerSkates(std::string vendor) :
        vendor(vendor),
        bootOneId(1),
        bootTwoId(2),
        destructorWork()
    {}

    RollerSkates(const RollerSkates& rhs) = default;
    RollerSkates& operator= (const RollerSkates& rhs) = default;

    RollerSkates(RollerSkates&& rhs) :
        vendor(rhs.vendor),
        bootOneId(rhs.bootOneId),
        bootTwoId(rhs.bootTwoId),
        destructorWork(rhs.destructorWork)
    {
        rhs.vendor = "";
        rhs.bootOneId = -1;
        rhs.bootTwoId = -1;
        rhs.destructorWork = {};
    }

    RollerSkates& operator= (RollerSkates&& rhs)
    {
        if (this != &rhs)
        {
            vendor = rhs.vendor;
            bootOneId = rhs.bootOneId;
            bootTwoId = rhs.bootTwoId;
            rhs.vendor = "";
            rhs.bootOneId = -1;
            rhs.bootTwoId = -1;
            rhs.destructorWork = {};
        }
        return *this;
    }

    ~RollerSkates() { destructorWork(); };

    std::string vendor;
    int8_t bootOneId;
    int8_t bootTwoId;
    std::function<void(void)> destructorWork;
};

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Construction_Destruction)
{
    int numDestructorsCalled = 0;

    {
        UniquePtr<RollerSkates> up1;
        EXPECT_EQ(up1.get(), nullptr);

        RollerSkates* rs = new RollerSkates("Rollerblade");
        rs->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        UniquePtr<RollerSkates> up2(rs);
        EXPECT_EQ(up2.get(), rs);
    }

    EXPECT_EQ(numDestructorsCalled, 1);
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_MoveConstruction)
{
    int numDestructorsCalled = 0;
    std::string VENDOR = "Sure-Grip";
    int8_t BOOT_ONE_ID = 12;
    int8_t BOOT_TWO_ID = 13;

    {
        RollerSkates* rs = new RollerSkates(VENDOR);
        UniquePtr<RollerSkates> up1(rs);
        up1->bootOneId = BOOT_ONE_ID;
        up1->bootTwoId = BOOT_TWO_ID;
        up1->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        UniquePtr<RollerSkates> up2(std::move(up1));

        EXPECT_EQ(up1.get(), nullptr);
        EXPECT_EQ(up2.get(), rs);
        EXPECT_EQ(up2->vendor, VENDOR);
        EXPECT_EQ(up2->bootOneId, BOOT_ONE_ID);
        EXPECT_EQ(up2->bootTwoId, BOOT_TWO_ID);
    }

    EXPECT_EQ(numDestructorsCalled, 1);
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_MoveAssignment)
{
    int numDestructorsCalled = 0;
    std::string VENDOR = "Moxi";
    int8_t BOOT_ONE_ID = 14;
    int8_t BOOT_TWO_ID = 15;

    {
        RollerSkates* rs = new RollerSkates(VENDOR);
        UniquePtr<RollerSkates> up1(rs);
        up1->bootOneId = BOOT_ONE_ID;
        up1->bootTwoId = BOOT_TWO_ID;
        up1->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        UniquePtr<RollerSkates> up2(new RollerSkates("Atom"));
        up2->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        up2 = std::move(up1);

        EXPECT_EQ(up1.get(), nullptr);
        EXPECT_EQ(up2.get(), rs);
        EXPECT_EQ(up2->vendor, VENDOR);
        EXPECT_EQ(up2->bootOneId, BOOT_ONE_ID);
        EXPECT_EQ(up2->bootTwoId, BOOT_TWO_ID);
    }

    EXPECT_EQ(numDestructorsCalled, 2);
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Release)
{
    int numDestructorsCalled = 0;
    RollerSkates* rs1 = new RollerSkates("Riedell");
    RollerSkates* rs2 = nullptr;
    {
        UniquePtr<RollerSkates> up(rs1);
        up->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        rs2 = up.release();

        EXPECT_EQ(up.get(), nullptr);
    }

    EXPECT_EQ(rs1, rs2);
    EXPECT_EQ(numDestructorsCalled, 0);
    delete rs1;
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Reset)
{
    int numDestructorsCalled = 0;
    {
        UniquePtr<RollerSkates> up1(new RollerSkates("Kryptonics"));
        up1->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        up1.reset();

        EXPECT_EQ(up1.get(), nullptr);

        up1 = UniquePtr<RollerSkates>(new RollerSkates("Kryptonics"));
        up1->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
        up1.reset(UniquePtr<RollerSkates>(new RollerSkates("Kryptonics")));
        up1->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };
    }

    EXPECT_EQ(numDestructorsCalled, 3);
}

TEST(BicyclesUniquePtrTestSuite, UniquePtr_Swap_BoolOperators)
{
    int numDestructorsCalled = 0;

    std::string VENDOR_A = "Fila";
    int8_t BOOT_ONE_ID_A = 20;
    int8_t BOOT_TWO_ID_A = 30;

    std::string VENDOR_B = "Luigino";
    int8_t BOOT_ONE_ID_B = 40;
    int8_t BOOT_TWO_ID_B = 50;

    {
        RollerSkates* rs1 = new RollerSkates(VENDOR_A);
        UniquePtr<RollerSkates> up1(rs1);
        up1->bootOneId = BOOT_ONE_ID_A;
        up1->bootTwoId = BOOT_TWO_ID_A;
        up1->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };

        RollerSkates* rs2 = new RollerSkates(VENDOR_B);
        UniquePtr<RollerSkates> up2(rs2);
        up2->bootOneId = BOOT_ONE_ID_B;
        up2->bootTwoId = BOOT_TWO_ID_B;
        up2->destructorWork = [&numDestructorsCalled](){ numDestructorsCalled++; };

        UniquePtr<RollerSkates>::swap(up1, up2);

        EXPECT_EQ(numDestructorsCalled, 0);
        EXPECT_EQ(up1.get(), rs2);
        EXPECT_EQ(up2.get(), rs1);
        EXPECT_EQ(up1->vendor, VENDOR_B);
        EXPECT_EQ(up2->vendor, VENDOR_A);
        EXPECT_EQ(up1->bootOneId, BOOT_ONE_ID_B);
        EXPECT_EQ(up1->bootTwoId, BOOT_TWO_ID_B);
        EXPECT_EQ(up2->bootOneId, BOOT_ONE_ID_A);
        EXPECT_EQ(up2->bootTwoId, BOOT_TWO_ID_A);

        up1.reset();
        EXPECT_EQ(numDestructorsCalled, 1);
        EXPECT_TRUE(up1 == nullptr);
        EXPECT_FALSE(up1 != nullptr);
        EXPECT_TRUE(up2);
        EXPECT_FALSE(up1);
    }

    EXPECT_EQ(numDestructorsCalled, 2);
}
