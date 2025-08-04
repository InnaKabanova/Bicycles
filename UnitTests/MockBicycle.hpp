#pragma once

#include "BicycleImpl.hpp"

#include <gmock/gmock.h>

namespace mybicycles
{
class MockBicycle : public BicycleImpl
{
public:
    MockBicycle(std::string vendor) :
        BicycleImpl(vendor)
    {}

    MockBicycle(std::string vendor, int16_t pressureFront, int16_t pressureRear) :
        BicycleImpl(vendor, pressureFront, pressureRear)
    {}

    MockBicycle(const MockBicycle& rhs) :
        BicycleImpl(rhs)
    {}

    MockBicycle& operator= (const MockBicycle& rhs)
    {
        BicycleImpl::operator=(rhs);
        return *this;
    }

    MockBicycle(MockBicycle&& rhs) :
        BicycleImpl(rhs)
    {}

    MockBicycle& operator=(MockBicycle&& rhs)
    {
        BicycleImpl::operator=(rhs);
        return *this;
    }

    virtual ~MockBicycle() { die(); }
    MOCK_METHOD0(die, void());
    MOCK_METHOD(void, ringBell, (), (const, override));
};

} // mybicycles
