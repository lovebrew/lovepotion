#include <utilities/sensor/gyroscope.hpp>

using namespace love;

Gyroscope::Gyroscope() : data {}
{}

Gyroscope::~Gyroscope()
{
    this->SetEnabled(false);
}
