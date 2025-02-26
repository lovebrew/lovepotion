#pragma once

#include "driver/EventQueue.tcc"

#include "modules/joystick/kpad/Joystick.hpp"
#include "modules/joystick/vpad/Joystick.hpp"

#include <vpad/input.h>

namespace love
{
    class EventQueue : public EventQueueBase<EventQueue>
    {
      public:
        EventQueue();

        ~EventQueue();

        void pollInternal() override;

      private:
        vpad::Joystick* gamepad;
        VPADTouchData previousTouch;
        bool wasTouched;
    };
} // namespace love
