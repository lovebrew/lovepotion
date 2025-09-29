#pragma once

#include "common/Map.hpp"
#include "common/Module.hpp"
#include "events.hpp"

#include <vector>

namespace love
{
    class Touch : public Module
    {
      public:
        enum DeviceType
        {
            DEVICE_TOUCHSCREEN,
            DEVICE_TOUCHPAD,
            DEVICE_TOUCHPAD_RELATIVE,
            DEVICE_MAX_ENUM
        };

        Touch();

        virtual ~Touch()
        {}

        const std::vector<Finger>& getTouches();

        const Finger& getTouch(int64_t id);

        void onEvent(SubEventType type, const Finger& info);

        // clang-format off
        STRINGMAP_DECLARE(DeviceTypes, DeviceType,
          { "touchscreen",       DEVICE_TOUCHSCREEN       },
          { "touchpad",          DEVICE_TOUCHPAD          },
          { "touchpad_relative", DEVICE_TOUCHPAD_RELATIVE }
        );
        // clang-format on

      private:
        std::vector<Finger> touches;
    };
} // namespace love
