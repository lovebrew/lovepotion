#pragma once

#include "driver/EventQueue.tcc"

#include <3ds.h>

namespace love
{
    class EventQueue : public EventQueueBase<EventQueue>
    {
      public:
        EventQueue();

        ~EventQueue();

        void pollInternal() override;

        struct
        {
            touchPosition current;
            touchPosition previous;
        } touchState;
    };
} // namespace love
