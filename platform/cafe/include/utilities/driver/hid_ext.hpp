#pragma once

#include <utilities/driver/hid.tcc>

#include <vpad/input.h>

namespace love
{
    template<>
    class HID<Console::CAFE> : public HID<Console::ALL>
    {
      public:
        static HID& Instance()
        {
            static HID instance;
            return instance;
        }

        ~HID();

        bool Poll(LOVE_Event* event);

      private:
        HID();

        VPADStatus vpad;
        VPADTouchData previousTouch;
    };
} // namespace love
