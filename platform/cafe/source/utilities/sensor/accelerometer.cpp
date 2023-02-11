#include <utilities/sensor/accelerometer.hpp>

using namespace love;

Accelerometer::Accelerometer() : data {}
{}

Accelerometer::~Accelerometer()
{
    this->SetEnabled(false);
}
