#pragma once

#include "common/Message.hpp"
#include "common/Module.hpp"

#include "events.hpp"

#include <mutex>
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

        void pump(float timeout = 0.0f);

        Message* wait();

      private:
        std::recursive_mutex mutex;
        std::queue<Message*> messages;

        LOVE_Event event;
    };
} // namespace love
