#ifndef BICYCLE_HPP
#define BICYCLE_HPP

#include <cstdint>

namespace mybicycles
{

constexpr uint8_t CITY_BIKE_NORMAL_PRESSURE_PSI = 50;

struct Tyre
{
    Tyre() = default;
    Tyre(int16_t pressure) : pressure(pressure) {}

    int16_t pressure = CITY_BIKE_NORMAL_PRESSURE_PSI;
};

/**
 * Interface of a helper class to test my re-invented self-written bicycles
 */
class Bicycle
{
public:
    virtual ~Bicycle() = default;
    virtual void ringBell() const = 0;
};

} // mybicycles

#endif // BICYCLE_HPP
