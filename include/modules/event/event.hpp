#pragma once

#include "common/Message.hpp"
#include "common/Module.hpp"

#include "events.hpp"

#include <queue>

namespace love
{
    class Event : public Module
    {
      public:
        Event();

        virtual ~Event();

        void push(Message* message);

        bool poll(Message*& message);

        void clear();

        void pump();

        Message* wait();

      private:
        std::mutex mutex;
        std::queue<Message*> messages;

        LOVE_Event event;
    };
} // namespace love
