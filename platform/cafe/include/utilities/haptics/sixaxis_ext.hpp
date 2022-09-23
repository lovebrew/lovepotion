#pragma once

#include <utilities/haptics/sixaxis.tcc>

#include <padscore/kpad.h>
#include <vpad/input.h>

namespace love
{
    template<>
    class SixAxis<Console::CAFE> : public SixAxis<Console::ALL>
    {
      public:
        SixAxis()
        {}

        SixAxis(bool isGamepad);

        void Update(VPADVec3D vector, SixAxisType type);

        void Update(KPADVec3D vector, SixAxisType type);

        Vector3 GetInfo(SixAxisType type);

      private:
        VPADVec3D accelerometer;
        VPADVec3D gyroscope;

        KPADVec3D kpadAccelerometer;

        bool isGamepad;
    };
} // namespace love
