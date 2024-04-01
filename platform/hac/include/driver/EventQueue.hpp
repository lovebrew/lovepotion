#pragma once

#include "driver/EventQueue.tcc"

#include <switch.h>

namespace love
{
    class EventQueue : public EventQueueBase<EventQueue>
    {
      public:
        EventQueue();

        ~EventQueue();

        void pollInternal() override;

      private:
        Event padStyleUpdates[0x08];
    };
} // namespace love
