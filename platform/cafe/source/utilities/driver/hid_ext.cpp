#include <modules/joystickmodule_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <padscore/kpad.h>
#include <proc_ui/procui.h>

#include <utilities/log/logfile.h>

#define Module() Module::GetInstance<JoystickModule<Console::CAFE>>(Module::M_JOYSTICK)

using namespace love;

HID<Console::CAFE>::HID() : previousTouch {}
{}

HID<Console::CAFE>::~HID()
{}

void HID<Console::CAFE>::CheckFocus()
{
    const auto status = ProcUIProcessMessages(true);

    if (status == PROCUI_STATUS_EXITING)
    {
        this->SendQuit();
        return;
    }

    switch (status)
    {
        case PROCUI_STATUS_IN_FOREGROUND:
        {
            this->SendFocus(true);
            break;
        }
        case PROCUI_STATUS_RELEASE_FOREGROUND:
        {
            this->SendFocus(false);
            break;
        }
        default:
            break;
    }
}

bool HID<Console::CAFE>::Poll(LOVE_Event* event)
{
    if (!this->events.empty())
    {
        *event = this->events.front();
        this->events.pop_front();

        return true;
    }

    if (this->hysteresis)
        return this->hysteresis = false;

    this->CheckFocus();

    if (Module())
    {
        auto* joystick = Module()->GetJoystickFromId(0);

        if (joystick)
        {
            joystick->Update();

            const auto tpNormal = joystick->GetTouchData();

            if (tpNormal.touched)
            {
                auto& newEvent = this->events.emplace_back();

                newEvent.type = TYPE_TOUCH;

                uint16_t dx = this->previousTouch.x - tpNormal.x;
                uint16_t dy = this->previousTouch.y - tpNormal.y;

                if (dx == 0 && dy == 0)
                    newEvent.subType = SUBTYPE_TOUCHPRESS;
                else
                    newEvent.subType = SUBTYPE_TOUCHMOVED;

                newEvent.touchFinger.id       = 0;
                newEvent.touchFinger.x        = tpNormal.x;
                newEvent.touchFinger.y        = tpNormal.y;
                newEvent.touchFinger.dx       = dx;
                newEvent.touchFinger.dy       = dy;
                newEvent.touchFinger.pressure = 1.0f;

                this->touchHeld     = true;
                this->previousTouch = tpNormal;
            }

            if (!tpNormal.touched && this->touchHeld)
            {
                auto& newEvent      = this->events.emplace_back();
                this->previousTouch = tpNormal;

                newEvent.type    = TYPE_TOUCH;
                newEvent.subType = SUBTYPE_TOUCHRELEASE;

                newEvent.touchFinger.id       = 0;
                newEvent.touchFinger.x        = tpNormal.x;
                newEvent.touchFinger.y        = tpNormal.y;
                newEvent.touchFinger.dx       = 0.0f;
                newEvent.touchFinger.dy       = 0.0f;
                newEvent.touchFinger.pressure = 1.0f;

                if (this->touchHeld)
                    this->touchHeld = false;
            }

            Joystick<>::JoystickInput input {};

            if (joystick->IsDown(input))
            {
                auto& newEvent = this->events.emplace_back();

                newEvent.type    = TYPE_GAMEPAD;
                newEvent.subType = SUBTYPE_GAMEPADDOWN;

                Joystick<>::GetConstant(input.button, newEvent.padButton.name);
                newEvent.padButton.id     = joystick->GetID();
                newEvent.padButton.button = input.buttonNumber;
            }

            if (joystick->IsUp(input))
            {
                auto& newEvent = this->events.emplace_back();

                newEvent.type    = TYPE_GAMEPAD;
                newEvent.subType = SUBTYPE_GAMEPADUP;

                Joystick<>::GetConstant(input.button, newEvent.padButton.name);
                newEvent.padButton.id     = joystick->GetID();
                newEvent.padButton.button = input.buttonNumber;
            }
        }
    }

    if (Module())
    {
        for (size_t index = 1; index < Module()->GetJoystickCount(); index++)
        {
            auto* joystick = Module()->GetJoystickFromId(index);

            if (joystick)
            {
                joystick->Update();
                Joystick<>::JoystickInput input {};

                if (joystick->IsDown(input))
                {
                    auto& newEvent = this->events.emplace_back();

                    newEvent.type    = TYPE_GAMEPAD;
                    newEvent.subType = SUBTYPE_GAMEPADDOWN;

                    Joystick<>::GetConstant(input.button, newEvent.padButton.name);
                    newEvent.padButton.id     = joystick->GetID();
                    newEvent.padButton.button = input.buttonNumber;
                }

                if (joystick->IsUp(input))
                {
                    auto& newEvent = this->events.emplace_back();

                    newEvent.type    = TYPE_GAMEPAD;
                    newEvent.subType = SUBTYPE_GAMEPADUP;

                    Joystick<>::GetConstant(input.button, newEvent.padButton.name);
                    newEvent.padButton.id     = joystick->GetID();
                    newEvent.padButton.button = input.buttonNumber;
                }
            }
        }
    }

    /* return our events */

    if (this->events.empty())
        return false;

    *event = this->events.front();
    this->events.pop_front();

    return this->hysteresis = true;
}
