#pragma once

#include <memory>
#include <vector>

#include <switch.h>

namespace love
{
    class SixAxis
    {
      public:
        struct SixAxisInfo
        {
            HidVector acceleration[2];
            HidVector velocity[2];
        };

        SixAxis(HidNpadIdType playerId, HidNpadStyleTag style);

        ~SixAxis();

        SixAxisInfo GetStateInfo();

      private:
        std::unique_ptr<HidSixAxisSensorHandle[]> handles;
        int handleCount;
    };
} // namespace love
