#pragma once

#include "modules/event/eventc.h"

namespace love
{
    class Event : public common::Event
    {
      public:
        Event();

        virtual ~Event();
    };
} // namespace love