#pragma once

#include "driver/EventQueue.tcc"
#include "modules/joystick/Joystick.hpp"

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
        Joystick* gamepad;
        VPADTouchData previousTouch;
    };
} // namespace love
