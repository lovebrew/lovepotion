#pragma once

#include <array>

#include <switch.h>

#include <utilities/driver/hid.tcc>
#include <utilities/npad.hpp>

namespace love
{
    template<>
    class HID<Console::HAC> : public HID<Console::ALL>
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

        static constexpr int MAX_TOUCHES = 16;

        void CheckFocus();

        HidTouchScreenState touchState;

        std::array<HidTouchState, MAX_TOUCHES> stateTouches;
        std::array<HidTouchState, MAX_TOUCHES> oldStateTouches;

        int previousTouchCount;

        std::vector<HidNpadIdType> previousJoystickState;
        std::array<::Event, npad::MAX_JOYSTICKS> statusEvents;
    };
} // namespace love
