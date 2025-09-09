#pragma once

#include "Bicycle.hpp"

#include <iostream>

namespace mybicycles
{

/**
 * Helper class to test my re-invented self-written bicycles
 */
class BicycleImpl : public Bicycle
{
    friend std::ostream& operator<<(std::ostream& os, const BicycleImpl& bike);

public:
    BicycleImpl(std::string vendor, bool debugLogsEnabled = false) :
        mVendor(vendor),
        mFrontTyre(),
        mRearTyre(),
        mDebugLogsEnabled(debugLogsEnabled)
    {
        if (mDebugLogsEnabled)
        {
            std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        }
    }

    BicycleImpl(std::string vendor, int16_t pressureFront, int16_t pressureRear, bool debugLogsEnabled = false) :
        mVendor(vendor),
        mFrontTyre(pressureFront),
        mRearTyre(pressureRear),
        mDebugLogsEnabled(debugLogsEnabled)
    {
        if (mDebugLogsEnabled)
        {
            std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        }
    }

    BicycleImpl(const BicycleImpl& rhs) :
        mVendor(rhs.mVendor),
        mFrontTyre(rhs.mFrontTyre),
        mRearTyre(rhs.mRearTyre),
        mDebugLogsEnabled(rhs.mDebugLogsEnabled)
    {
        if (mDebugLogsEnabled)
        {
            std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        }
    }

    BicycleImpl& operator= (const BicycleImpl& rhs)
    {
        if (mDebugLogsEnabled)
        {
            std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        }

        // Self-assignment is safe here
        mVendor = rhs.mVendor;
        mFrontTyre = rhs.mFrontTyre;
        mRearTyre = rhs.mRearTyre;
        mDebugLogsEnabled = rhs.mDebugLogsEnabled;
        return *this;
    }

    BicycleImpl(BicycleImpl&& rhs) :
        mVendor(rhs.mVendor),
        mFrontTyre(rhs.mFrontTyre),
        mRearTyre(rhs.mRearTyre),
        mDebugLogsEnabled(rhs.mDebugLogsEnabled)
    {
        rhs.reset();
    }

    BicycleImpl& operator=(BicycleImpl&& rhs)
    {
        if (mDebugLogsEnabled)
        {
            std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        }

        if (this != &rhs)
        {
            mVendor = rhs.mVendor;
            mFrontTyre = rhs.mFrontTyre;
            mRearTyre = rhs.mRearTyre;
            mDebugLogsEnabled = rhs.mDebugLogsEnabled;
            rhs.reset();
        }
        return *this;
    }

    virtual ~BicycleImpl() override
    {
        if (mDebugLogsEnabled)
        {
            std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
        }
    }

    bool operator<(const BicycleImpl& rhs) const {
        return mVendor < rhs.mVendor;
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

    bool mDebugLogsEnabled;

private:
    void reset()
    {
        mVendor = "";
        mFrontTyre.pressure = 0;
        mRearTyre.pressure = 0;
        mDebugLogsEnabled = false;
    }

};

inline std::ostream& operator<<(std::ostream& os, const BicycleImpl& bc) {
    os << bc.mVendor << ", front tyre " << bc.mFrontTyre << ", rear tyre " << bc.mRearTyre;
    return os;
}

} // mybicycles
