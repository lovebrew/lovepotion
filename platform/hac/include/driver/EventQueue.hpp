#pragma once

#include "driver/EventQueue.tcc"

#include <switch.h>
#define MAX_TOUCHES 16

namespace love
{
    class EventQueue : public EventQueueBase<EventQueue>
    {
      public:
        EventQueue();

        ~EventQueue();

        void pollInternal() override;

      private:
        std::array<Event, 8> padStyleUpdates;

        HidTouchScreenState touchState;
        int previousTouchCount;

        std::array<HidTouchState, MAX_TOUCHES> touches;
        std::array<HidTouchState, MAX_TOUCHES> oldTouches;
    };
} // namespace love
