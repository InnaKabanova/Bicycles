#ifndef BICYCLE_HPP
#define BICYCLE_HPP

#include <cstdint>
#include <string>

namespace mybicycles
{

constexpr uint8_t CITY_BIKE_NORMAL_PRESSURE_PSI = 50;

struct Tyre
{
    uint8_t pressure = CITY_BIKE_NORMAL_PRESSURE_PSI;
};

/**
 * Helper class to test my re-invented self-written bicycles
 */
class Bicycle
{
public:
    Bicycle(std::string vendor);
    ~Bicycle();
    Bicycle(const Bicycle& rhs);
    Bicycle& operator= (const Bicycle& rhs);
    Bicycle(Bicycle&& rhs);
    Bicycle& operator=(Bicycle&& rhs);

    void ringBell() const;

private:
    std::string mVendor;
    Tyre mFrontTyre;
    Tyre mRearTyre;
};

} // mybicycles

#endif // BICYCLE_HPP
