#pragma once

#include "common/Singleton.tcc"
#include "events.hpp"

#include <list>

namespace love
{
    template<class T>
    class EventQueueBase : public Singleton<T>
    {
      public:
        EventQueueBase() : hysteresis(false), touchHeld(false), focused(false), events {}
        {}

        bool poll(LOVE_Event* event)
        {
            if (!this->events.empty())
            {
                *event = this->events.front();
                this->events.pop_front();
                return true;
            }

            if (this->hysteresis)
                return this->hysteresis = false;

            this->pollInternal();

            if (this->events.empty())
                return false;

            *event = this->events.front();
            this->events.pop_front();

            return this->hysteresis = true;
        }

        void sendFocus(bool focus)
        {
            if (this->focused == focus)
                return;

            auto& event = this->events.emplace_back();

            event.type    = TYPE_WINDOW;
            event.subtype = focus ? SUBTYPE_FOCUS_GAINED : SUBTYPE_FOCUS_LOST;
            this->focused = focus;
        }

        void sendQuit()
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GENERAL;
            event.subtype = SUBTYPE_QUIT;
        }

        void sendResize(int width, int height)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_WINDOW;
            event.subtype = SUBTYPE_RESIZE;
            event.resize  = { width, height };
        }

      private:
        bool hysteresis;

      protected:
        bool touchHeld;
        bool focused;

        std::list<LOVE_Event> events;

        virtual void pollInternal() = 0;

        void sendTouchEvent(SubEventType type, size_t id, float x, float y, float dx, float dy,
                            float pressure)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_TOUCH;
            event.subtype = type;
            event.finger  = { id, x, y, dx, dy, pressure };
        }
    };
} // namespace love
