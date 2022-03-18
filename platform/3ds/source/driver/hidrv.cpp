#include "driver/hidrv.h"
#include "modules/joystick/joystick.h"
#include "objects/gamepad/gamepad.h"

#include <string.h>

#define MODULE() love::Module::GetInstance<love::Joystick>(love::Module::M_JOYSTICK)

using namespace love::driver;

Hidrv::Hidrv() : oldSticks {}, oldTouchState {}, isTouchHeld(false)
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

    u32 touchDown     = hidKeysDown();
    u32 touchHeld     = hidKeysHeld();
    u32 touchReleased = hidKeysUp();

    if (touchDown & KEY_TOUCH || touchHeld & KEY_TOUCH)
        hidTouchRead(&this->touchState);

    if (!this->isTouchHeld && (touchDown & KEY_TOUCH))
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

            this->isTouchHeld = true;
        }
    }

    if (touchReleased & KEY_TOUCH)
    {
        auto& newEvent      = this->events.emplace_back();
        this->oldTouchState = this->touchState;

        newEvent.type = TYPE_TOUCHRELEASE;

        newEvent.touch.id       = 0;
        newEvent.touch.x        = this->oldTouchState.px;
        newEvent.touch.y        = this->oldTouchState.py;
        newEvent.touch.dx       = 0.0f;
        newEvent.touch.dy       = 0.0f;
        newEvent.touch.pressure = 0.0f;

        if (this->isTouchHeld)
            this->isTouchHeld = false;
    }

    Gamepad* gamepad = MODULE()->GetJoystickFromID(0);

    if (gamepad)
    {
        gamepad->Update();

        /* handle button inputs */

        Gamepad::ButtonMapping button;

        const auto& mappings = gamepad->GetButtonMapping();
        const auto entries   = mappings.GetEntries();

        for (size_t index = 0; index < entries.second; index++)
        {
            if (gamepad->IsDown(index, button))
            {
                auto& newEvent = this->events.emplace_back();

                newEvent.type = TYPE_GAMEPADDOWN;

                newEvent.button.name   = button.first;
                newEvent.button.which  = gamepad->GetID();
                newEvent.button.button = button.second;
            }
        }

        for (size_t index = 0; index < entries.second; index++)
        {
            if (gamepad->IsUp(index, button))
            {
                auto& newEvent = this->events.emplace_back();

                newEvent.type = TYPE_GAMEPADUP;

                newEvent.button.name   = button.first;
                newEvent.button.which  = gamepad->GetID();
                newEvent.button.button = button.second;
            }
        }

        /* handle trigger inputs */
        for (size_t i = 5; i <= 6; i++)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type       = TYPE_GAMEPADAXIS;
            newEvent.axis.which = gamepad->GetID();

            newEvent.axis.axis   = (i == 5) ? "triggerleft" : "triggerright";
            newEvent.axis.value  = gamepad->GetAxis(i);
            newEvent.axis.number = i;
        }

        /* handle stick inputs */
        for (size_t i = 1; i <= 4; i++)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type       = TYPE_GAMEPADAXIS;
            newEvent.axis.which = gamepad->GetID();

            const char* axis = nullptr;
            if (i < 3) // left
            {
                if ((i % 2) != 0)
                    axis = "leftx";
                else
                    axis = "lefty";
            }
            else
            {
                if ((i % 2) != 0)
                    axis = "rightx";
                else
                    axis = "righty";
            }

            newEvent.axis.axis   = axis;
            newEvent.axis.value  = gamepad->GetAxis(i);
            newEvent.axis.number = i;
        }
    }

    if (this->events.empty())
        return false;

    *event = this->events.front();
    this->events.pop_front();

    return this->hysteresis = true;
}
