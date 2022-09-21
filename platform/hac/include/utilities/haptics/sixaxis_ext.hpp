#pragma once

#include <utilities/haptics/sixaxis.tcc>

#include <switch.h>

#include <vector>

namespace love
{
    template<>
    class SixAxis<Console::HAC> : public SixAxis<Console::ALL>
    {
      public:
        SixAxis()
        {}

        SixAxis(HidNpadIdType playerId, HidNpadStyleTag style);

        SixAxis& operator=(SixAxis&& other);

        ~SixAxis();

        bool Start();

        void Stop();

        std::vector<Vector3> GetStateInfo(SixAxis<>::SixAxisType type);

      private:
        std::unique_ptr<HidSixAxisSensorHandle[]> handles;

        HidNpadIdType playerId;
        HidNpadStyleTag style;
    };
} // namespace love
