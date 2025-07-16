#ifndef BICYCLE_IMPL_HPP
#define BICYCLE_IMPL_HPP

#include "Bicycle.hpp"

#include <iostream>

namespace mybicycles
{

/**
 * Helper class to test my re-invented self-written bicycles
 */
class BicycleImpl : public Bicycle
{
public:
    BicycleImpl(std::string vendor) :
        mVendor(vendor),
        mFrontTyre(),
        mRearTyre()
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
    }

    BicycleImpl(std::string vendor, int16_t pressureFront, int16_t pressureRear) :
        mVendor(vendor),
        mFrontTyre(pressureFront),
        mRearTyre(pressureRear)
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
    }

    BicycleImpl(const BicycleImpl& rhs) :
        mVendor(rhs.mVendor),
        mFrontTyre(rhs.mFrontTyre),
        mRearTyre(rhs.mRearTyre)
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
    }

    BicycleImpl& operator= (const BicycleImpl& rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        // Self-assignment is safe here
        mVendor = rhs.mVendor;
        mFrontTyre = rhs.mFrontTyre;
        mRearTyre = rhs.mRearTyre;
        return *this;
    }

    BicycleImpl(BicycleImpl&& rhs) :
        mVendor(rhs.mVendor),
        mFrontTyre(rhs.mFrontTyre),
        mRearTyre(rhs.mRearTyre)
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        rhs.mVendor = "";
        rhs.mFrontTyre.pressure = 0;
        rhs.mRearTyre.pressure = 0;
    }

    BicycleImpl& operator=(BicycleImpl&& rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        if (this != &rhs)
        {
            mVendor = rhs.mVendor;
            mFrontTyre = rhs.mFrontTyre;
            mRearTyre = rhs.mRearTyre;
            rhs.mVendor = "";
            rhs.mFrontTyre.pressure = 0;
            rhs.mRearTyre.pressure = 0;
        }
        return *this;
    }

    virtual ~BicycleImpl() override
    {
        std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
    }

    virtual void ringBell() const override
    {
        std::cout << "Caution! Bike " << mVendor << " is on its way!" << std::endl;
    }

    std::string getVendor() const
    {
        return mVendor;
    }

    int16_t getPressureFront() const
    {
        return mFrontTyre.pressure;
    }

    int16_t getPressureRear() const
    {
        return mRearTyre.pressure;
    }

protected:
    std::string mVendor;
    mybicycles::Tyre mFrontTyre;
    mybicycles::Tyre mRearTyre;
};

} // mybicycles

#endif // BICYCLE_IMPL_HPP
