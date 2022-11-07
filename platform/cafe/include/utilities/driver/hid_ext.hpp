#pragma once

#include <utilities/driver/hid.tcc>

#include <padscore/wpad.h>
#include <vpad/input.h>

#include <vector>

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

        void CheckFocus();

        void CheckSoftwareKeyboard(VPADStatus status);

        VPADTouchData previousTouch;
    };
} // namespace love
