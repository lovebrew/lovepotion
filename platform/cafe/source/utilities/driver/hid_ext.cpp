#include <modules/joystickmodule_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#define Module() Module::GetInstance<JoystickModule<Console::CAFE>>(Module::M_JOYSTICK)

using namespace love;

HID<Console::CAFE>::HID()
{
    VPADInit();
    KPADInit();
}

HID<Console::CAFE>::~HID()
{
    VPADShutdown();
    KPADShutdown();
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

    VPADReadError error {};
    VPADRead(VPAD_CHAN_0, &this->vpad, 1, &error);

    if (this->vpad.tpNormal.touched)
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_TOUCH;

        uint16_t dx = this->previousTouch.x - this->vpad.tpNormal.x;
        uint16_t dy = this->previousTouch.y - this->vpad.tpNormal.y;

        if (dx == 0 && dy == 0)
            newEvent.subType = SUBTYPE_TOUCHPRESS;
        else
            newEvent.subType = SUBTYPE_TOUCHMOVED;

        newEvent.touchFinger.id       = 0;
        newEvent.touchFinger.x        = this->vpad.tpNormal.x;
        newEvent.touchFinger.y        = this->vpad.tpNormal.y;
        newEvent.touchFinger.dx       = dx;
        newEvent.touchFinger.dy       = dy;
        newEvent.touchFinger.pressure = 1.0f;

        this->touchHeld     = true;
        this->previousTouch = this->vpad.tpNormal;
    }

    if (!this->vpad.tpNormal.touched && this->touchHeld)
    {
        auto& newEvent      = this->events.emplace_back();
        this->previousTouch = this->vpad.tpNormal;

        newEvent.type    = TYPE_TOUCH;
        newEvent.subType = SUBTYPE_TOUCHRELEASE;

        newEvent.touchFinger.id       = 0;
        newEvent.touchFinger.x        = this->vpad.tpNormal.x;
        newEvent.touchFinger.y        = this->vpad.tpNormal.y;
        newEvent.touchFinger.dx       = 0.0f;
        newEvent.touchFinger.dy       = 0.0f;
        newEvent.touchFinger.pressure = 1.0f;

        if (this->touchHeld)
            this->touchHeld = false;
    }

    if (Module())
    {
        for (size_t index = 0; index < Module()->GetJoystickCount(); index++)
        {
            auto* joystick = Module()->GetJoystickFromId(index);

            if (joystick)
            {
                joystick->Update(this->vpad);
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
