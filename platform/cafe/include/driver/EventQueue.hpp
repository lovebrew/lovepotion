#pragma once

#include "driver/EventQueue.tcc"

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
        VPADTouchData previousTouch;
    };
} // namespace love
