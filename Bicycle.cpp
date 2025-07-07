#include <iostream>

#include "Bicycle.hpp"

namespace mybicycles
{

Bicycle::Bicycle(std::string vendor) :
    mVendor(vendor),
    mFrontTyre(),
    mRearTyre()
{
    std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
}

Bicycle::~Bicycle()
{
    std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
}

Bicycle::Bicycle(const Bicycle& rhs) :
    mVendor(rhs.mVendor),
    mFrontTyre(rhs.mFrontTyre),
    mRearTyre(rhs.mRearTyre)
{
    std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
}

Bicycle& Bicycle::operator= (const Bicycle& rhs)
{
    std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
    // Self-assignment is safe here
    mVendor = rhs.mVendor;
    mFrontTyre = rhs.mFrontTyre;
    mRearTyre = rhs.mRearTyre;
    return *this;
}

Bicycle::Bicycle(Bicycle&& rhs) :
    mVendor(rhs.mVendor),
    mFrontTyre(rhs.mFrontTyre),
    mRearTyre(rhs.mRearTyre)
{
    std::cout << __PRETTY_FUNCTION__ << " | " << mVendor << std::endl;
    rhs.mVendor = "";
    rhs.mFrontTyre.pressure = 0;
    rhs.mRearTyre.pressure = 0;
}

Bicycle& Bicycle::operator= (Bicycle&& rhs)
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

void Bicycle::ringBell() const
{
    std::cout << "Caution! Bike " << mVendor << " is on its way!" << std::endl;
}

} // mybicycles
