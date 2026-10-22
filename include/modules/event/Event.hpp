#pragma once

#include "common/Message.hpp"
#include "common/Module.hpp"

#include "events.hpp"
#include "modules/thread/threads.hpp"

#include <deque>

namespace love
{
    class Event : public Module
    {
      public:
        Event();

        virtual ~Event();

        void push(Message* message);

        void push(Message* message, bool pushFront);

        bool poll(Message*& message);

        void clear();

        void pump(float timeout = 0.0f);

        Message* wait();

      private:
        Mutex mutex;
        std::deque<Message*> messages;

        LOVE_Event event;
    };
} // namespace love
