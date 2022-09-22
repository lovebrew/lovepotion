#pragma once

#include <common/console.hpp>

#include <list>

#include <objects/joystick/joystick.tcc>
#include <utilities/driver/events.hpp>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class HID
    {
      public:
        static HID& Instance()
        {
            static HID instance;
            return instance;
        }

        HID() : touchHeld(false), hysteresis(false), focused(false), events(), stickValues {}
        {}

        void SendFocus(bool focus)
        {
            if (this->focused == focus)
                return;

            auto& event = this->events.emplace_back();

            event.type    = TYPE_WINDOW;
            event.subType = (focus) ? SUBTYPE_FOCUS_GAINED : SUBTYPE_FOCUS_LOST;
            this->focused = focus;
        }

        void SendLowMemory()
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GENERAL;
            event.subType = SUBTYPE_LOWMEMORY;
        }

        void SendQuit()
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GENERAL;
            event.subType = SUBTYPE_QUIT;
        }

        void SendResize(int width, int height)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_WINDOW;
            event.subType = SUBTYPE_RESIZE;

            event.size.width  = width;
            event.size.height = height;
        }

        void SendJoystickStatus(size_t id, bool connected)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GAMEPAD;
            event.subType = (connected) ? SUBTYPE_GAMEPADADDED : SUBTYPE_GAMEPADREMOVED;

            event.padStatus.id = id;
        }

        void SendJoystickUpdated(size_t id)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GAMEPAD;
            event.subType = SUBTYPE_GAMEPADUPDATED;

            event.padStatus.id = id;
        }

      protected:
        bool touchHeld;
        bool hysteresis;
        bool focused;

        std::list<LOVE_Event> events;
        std::array<float, Joystick<>::GamepadAxis::GAMEPAD_AXIS_MAX_ENUM> stickValues;
    };
} // namespace love
