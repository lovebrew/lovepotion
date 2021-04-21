#include "driver/hidrv.h"
#include "objects/gamepad/gamepad.h"
#include "modules/joystick/joystick.h"

#include <string.h>

#define MODULE() love::Module::GetInstance<love::Joystick>(love::Module::M_JOYSTICK)

using namespace love::driver;

Hidrv::Hidrv() : oldSticks {}, oldTouchState {}, touchHeld(false)
{}

bool Hidrv::Poll(LOVE_Event* event)
{
    if (!this->events.empty())
    {
        *event = this->events.front();
        this->events.pop_front();

        return true;
    }

    if (this->hysteresis)
        return this->hysteresis = false;

    hidScanInput();

    /* touch screen */

    hidTouchRead(&this->touchState);

    u32 touchDown     = hidKeysDown();
    u32 touchHeld     = hidKeysHeld();
    u32 touchReleased = hidKeysUp();

    if (!this->touchHeld && (touchDown & KEY_TOUCH))
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_TOUCHPRESS;

        newEvent.touch.id       = 0;
        newEvent.touch.x        = this->touchState.px;
        newEvent.touch.y        = this->touchState.py;
        newEvent.touch.dx       = 0.0f;
        newEvent.touch.dy       = 0.0f;
        newEvent.touch.pressure = 1.0f;

        this->oldTouchState = this->touchState;
    }

    if (touchHeld & KEY_TOUCH)
    {
        float dx = this->oldTouchState.px - this->touchState.px;
        float dy = this->oldTouchState.py - this->touchState.py;

        if (dx != 0.0f || dy != 0.0f)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_TOUCHMOVED;

            newEvent.touch.id       = 0;
            newEvent.touch.x        = this->touchState.px;
            newEvent.touch.y        = this->touchState.py;
            newEvent.touch.dx       = dx;
            newEvent.touch.dy       = dy;
            newEvent.touch.pressure = 1.0f;

            this->touchHeld = true;
        }
    }

    if (touchReleased & KEY_TOUCH)
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_TOUCHRELEASE;

        newEvent.touch.id       = 0;
        newEvent.touch.x        = this->oldTouchState.px;
        newEvent.touch.y        = this->oldTouchState.py;
        newEvent.touch.dx       = 0.0f;
        newEvent.touch.dy       = 0.0f;
        newEvent.touch.pressure = 0.0f;

        this->oldTouchState = this->touchState;
    }


    Gamepad* gamepad = MODULE()->GetJoystickFromID(0);

    if (gamepad && gamepad->IsConnected())
    {
        /* handle button inputs */

        std::pair<const char*, size_t> button;

        if (gamepad->IsDown(button))
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADDOWN;

            newEvent.button.name   = button.first;
            newEvent.button.which  = gamepad->GetID();
            newEvent.button.button = button.second;
        }

        if (gamepad->IsUp(button))
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADUP;

            newEvent.button.name   = button.first;
            newEvent.button.which  = gamepad->GetID();
            newEvent.button.button = button.second;
        }
    }

    /* handle trigger inputs */

    /* handle stick inputs */

    if (this->events.empty())
        return false;

    *event = this->events.front();
    this->events.pop_front();

    return this->hysteresis = true;
}
