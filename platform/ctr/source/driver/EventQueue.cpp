#include "driver/EventQueue.hpp"

#include "modules/joystick/Joystick.hpp"
#include "modules/joystick/JoystickModule.hpp"

#include <vector>

using namespace love;

#define JOYSTICK_MODULE() Module::getInstance<JoystickModule>(Module::M_JOYSTICK)

static aptHookCookie s_aptHookCookie;

static void aptEventHook(const APT_HookType type, void*)
{
    switch (type)
    {
        case APTHOOK_ONSLEEP:
        case APTHOOK_ONSUSPEND:
        {
            love::EventQueue::getInstance().sendFocus(false);
            break;
        }
        case APTHOOK_ONWAKEUP:
        case APTHOOK_ONRESTORE:
        {
            love::EventQueue::getInstance().sendFocus(true);
            break;
        }
        case APTHOOK_ONEXIT:
        {
            love::EventQueue::getInstance().sendQuit();
            break;
        }
        default:
            break;
    }
}

namespace love
{
    EventQueue::EventQueue() : touchState {}
    {
        aptHook(&s_aptHookCookie, aptEventHook, nullptr);
    }

    EventQueue::~EventQueue()
    {
        aptUnhook(&s_aptHookCookie);
    }

    void EventQueue::pollInternal()
    {
        hidScanInput();

        const auto down = hidKeysDown();
        const auto held = hidKeysHeld();
        const auto up   = hidKeysUp();

        if (down & KEY_TOUCH || held & KEY_TOUCH)
            hidTouchRead(&this->touchState.current);

        if (down & KEY_TOUCH)
        {
            float x = this->touchState.current.px, y = this->touchState.current.py;
            this->sendTouchEvent(SUBTYPE_TOUCHPRESS, 0, x, y, 0.0f, 0.0, 1.0f);
        }
        else if (held & KEY_TOUCH)
        {
            float x = this->touchState.current.px, y = this->touchState.current.py;

            float dx = this->touchState.current.px - this->touchState.previous.px;
            float dy = this->touchState.current.py - this->touchState.previous.py;

            if (dx != 0.0f || dy != 0.0f)
                this->sendTouchEvent(SUBTYPE_TOUCHMOVED, 0, x, y, dx, dy, 1.0f);

            this->touchState.previous = this->touchState.current;
        }

        if (up & KEY_TOUCH)
        {
            float x = this->touchState.current.px, y = this->touchState.current.py;
            this->sendTouchEvent(SUBTYPE_TOUCHRELEASE, 0, x, y, 0.0f, 0.0, 0.0f);
        }

        const auto* joystick = JOYSTICK_MODULE()->getJoystick(0);

        if (!joystick)
            return;

        for (int input = 0; input < Joystick::GAMEPAD_BUTTON_MAX_ENUM; input++)
        {
            std::vector<Joystick::GamepadButton> inputs = { Joystick::GamepadButton(input) };

            if (joystick->isDown(inputs))
                this->sendGamepadButtonEvent(SUBTYPE_GAMEPADDOWN, 0, input);

            if (joystick->isUp(inputs))
                this->sendGamepadButtonEvent(SUBTYPE_GAMEPADUP, 0, input);
        }

        for (int input = 0; input < Joystick::GAMEPAD_AXIS_MAX_ENUM; input++)
        {
            if (joystick->isAxisChanged(Joystick::GamepadAxis(input)))
            {
                float value = joystick->getAxis(Joystick::GamepadAxis(input));
                this->sendGamepadAxisEvent(0, input, value);
            }

            Joystick::HidAxisType value;
            Joystick::getConstant(Joystick::GamepadAxis(input), value);

            if (hidKeysUp() & value)
                this->sendGamepadAxisEvent(0, input, 0.0f);
        }

        for (int input = 0; input < Sensor::SENSOR_MAX_ENUM; input++)
        {
            if (!joystick->isSensorEnabled(Sensor::SensorType(input)))
                continue;

            auto data = joystick->getSensorData(Sensor::SensorType(input));
            this->sendGamepadSensorEvent(0, input, data);
        }
    }
} // namespace love
