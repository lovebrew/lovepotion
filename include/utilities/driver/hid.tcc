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

        HID() : hysteresis(false), touchHeld(false), focused(false), events()
        {}

        bool Poll(LOVE_Event* event)
        {
            if (!this->events.empty())
            {
                *event = this->events.front();
                this->events.pop_front();

                return true;
            }

            if (this->hysteresis)
                return this->hysteresis = false;

            this->_Poll();

            /* return our events */

            if (this->events.empty())
                return false;

            *event = this->events.front();
            this->events.pop_front();

            return this->hysteresis = true;
        }

        void SendFocus(bool focus)
        {
            if (this->focused == focus)
                return;

            auto& event = this->events.emplace_back();

            event.type    = TYPE_WINDOW;
            event.subType = (focus) ? SUBTYPE_FOCUS_GAINED : SUBTYPE_FOCUS_LOST;
            this->focused = focus;
        }

        void SendQuit()
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GENERAL;
            event.subType = SUBTYPE_QUIT;
        }

        void SendTextInput(std::string_view string)
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_KEYBOARD;
            event.subType = SUBTYPE_TEXTINPUT;

            event.keyboard.text = string;
        }

      protected:
        void SendLowMemory()
        {
            auto& event = this->events.emplace_back();

            event.type    = TYPE_GENERAL;
            event.subType = SUBTYPE_LOWMEMORY;
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

        void SendJoystickSensorUpdated(size_t id, Sensor::SensorType type, std::vector<float> data)
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

        void SendTouchEvent(SubEventType type, size_t id, float x, float y, float dx, float dy,
                            float pressure)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_TOUCH;
            newEvent.subType = type;

            newEvent.touchFinger.id       = id;
            newEvent.touchFinger.x        = x;
            newEvent.touchFinger.y        = y;
            newEvent.touchFinger.dx       = dx;
            newEvent.touchFinger.dy       = dy;
            newEvent.touchFinger.pressure = pressure;
        }

        void SendGamepadPress(bool pressed, size_t id, Joystick<>::GamepadButton button,
                              int buttonIndex)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_GAMEPAD;
            newEvent.subType = pressed ? SUBTYPE_GAMEPADDOWN : SUBTYPE_GAMEPADUP;

            newEvent.padButton.name   = *Joystick<>::buttonTypes.ReverseFind(button);
            newEvent.padButton.id     = id;
            newEvent.padButton.button = buttonIndex;
        }

        void SendGamepadAxis(size_t id, Joystick<>::GamepadAxis axis, int axisIndex, float value)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_GAMEPAD;
            newEvent.subType = SUBTYPE_GAMEPADAXIS;

            newEvent.padAxis.id = id;

            const char* axisName  = *Joystick<>::axisTypes.ReverseFind(axis);
            newEvent.padAxis.name = axisName;

            newEvent.padAxis.axis  = (int)axis;
            newEvent.padAxis.value = value;
        }

      private:
        bool hysteresis;

      protected:
        bool touchHeld;
        bool focused;

        std::list<LOVE_Event> events;

        virtual void _Poll() = 0;
    };
} // namespace love
