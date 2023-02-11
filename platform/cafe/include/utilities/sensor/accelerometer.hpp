#pragma once

#include <common/vector.hpp>

#include <utilities/sensor/sensorbase.hpp>

#include <padscore/kpad.h>
#include <vpad/input.h>

namespace love
{
    class Accelerometer : public SensorBase
    {
      public:
        Accelerometer();

        ~Accelerometer();

        void Update(VPADVec3D input)
        {
            this->data = { input.x, input.y, input.z };
        }

        void Update(KPADVec3D input)
        {
            this->data = { input.x, input.y, input.z };
        }

        std::vector<float> GetData() override
        {
            return this->data;
        }

      private:
        std::vector<float> data;
    };
} // namespace love
