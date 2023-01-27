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

        void SendTextInput(std::string_view string)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_KEYBOARD;
            event.subType = SUBTYPE_TEXTINPUT;

            event.keyboard.text = string;
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

        void SendJoystickSensorUpdated(size_t id, Sensor::SensorType type,
                                       std::array<float, 3> data)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GAMEPAD;
            event.subType = SUBTYPE_GAMEPADSENSORUPDATED;

            event.padSensor.id   = id;
            event.padSensor.type = type;
            event.padSensor.data = data;
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
        float stickValues[5][Joystick<>::GAMEPAD_AXIS_MAX_ENUM];
    };
} // namespace love
