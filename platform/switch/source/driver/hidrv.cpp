#include "driver/hidrv.h"
#include "objects/gamepad/gamepad.h"

#include "modules/joystick/joystick.h"

#include <string.h>

#include "debug/logger.h"
#include "deko3d/deko.h"

using namespace love::driver;

#define MODULE() love::Module::GetInstance<love::Joystick>(love::Module::M_JOYSTICK)

Hidrv::Hidrv() : touchState {}, oldStateTouches {}, currentPadIndex(0), prevTouchCount(0)
{}

void Hidrv::CheckFocus()
{
    bool focused = (appletGetFocusState() == AppletFocusState_InFocus);

    uint32_t message = 0;
    Result res       = appletGetMessage(&message);

    if (R_SUCCEEDED(res))
    {
        bool shouldClose = !appletProcessMessage(message);

        if (shouldClose)
        {
            this->SendQuit();
            return;
        }

        switch (message)
        {
            case AppletMessage_FocusStateChanged:
            {

                bool oldFocus          = focused;
                AppletFocusState state = appletGetFocusState();

                focused = (state == AppletFocusState_InFocus);

                this->SendFocus(focused);

                if (focused == oldFocus)
                    break;

                if (focused)
                    appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
                else
                    appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleepNotify);

                break;
            }
            case AppletMessage_OperationModeChanged:
            {
                auto size = ::deko3d::Instance().OnOperationMode(appletGetOperationMode());
                this->SendResize(size.first, size.second);

                break;
            }
            default:
                break;
        }
    }
}

bool Hidrv::Poll(LOVE_Event* event)
{
    if (!this->events.empty())
    {
        *event = this->events.front();
        this->events.pop_front();

        return true;
    }

    if (this->hysteresis)
    {
        this->hysteresis = false;
        return false;
    }

    /* applet focus handling */
    this->CheckFocus();

    /* touch screen */
    hidGetTouchScreenStates(&touchState, 1);
    int touchCount = touchState.count;

    if (touchCount > 0)
    {
        for (int id = 0; id < touchCount; id++)
        {
            auto& newEvent = this->events.emplace_back();

            if (touchCount > this->prevTouchCount && id >= this->prevTouchCount)
            {
                this->stateTouches[id] = this->touchState.touches[id]; //< read the touches
                this->oldStateTouches[id] =
                    this->stateTouches[id]; //< set old touches to newly read ones

                newEvent.type = TYPE_TOUCHPRESS;
            }
            else
            {
                this->oldStateTouches[id] =
                    this->stateTouches[id]; //< set old touches to currently read ones
                this->stateTouches[id] = this->touchState.touches[id]; //< read the touches

                newEvent.type = TYPE_TOUCHMOVED;
            }

            newEvent.touch.id = id;

            newEvent.touch.x = this->stateTouches[id].x;
            newEvent.touch.y = this->stateTouches[id].y;

            newEvent.touch.dx = (s32)this->stateTouches[id].x - this->oldStateTouches[id].x;
            newEvent.touch.dy = (s32)this->stateTouches[id].y - this->oldStateTouches[id].y;

            newEvent.touch.pressure = 1.0f;

            if (newEvent.type == TYPE_TOUCHMOVED && !newEvent.touch.dx && !newEvent.touch.dy)
            {
                this->events.pop_back();
                continue;
            }
        }
    }

    if (touchCount < this->prevTouchCount)
    {
        for (int id = 0; id < prevTouchCount; ++id)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_TOUCHRELEASE;

            newEvent.touch.id       = id;
            newEvent.touch.x        = this->stateTouches[id].x;
            newEvent.touch.y        = this->stateTouches[id].y;
            newEvent.touch.dx       = 0.0f;
            newEvent.touch.dy       = 0.0f;
            newEvent.touch.pressure = 0.0f;
        }
    }

    this->prevTouchCount = touchCount;

    /* shouldn't happen, but eh */
    if (!MODULE())
        return false;

    int newIndex = -1;
    if ((newIndex = MODULE()->CheckGamepadAdded()) != -1)
    {
        auto& newEvent           = this->events.emplace_back();
        newEvent.type            = TYPE_GAMEPADADDED;
        newEvent.padStatus.which = newIndex;
    }

    if ((newIndex = MODULE()->CheckGamepadRemoved()) != -1)
    {
        auto& newEvent           = this->events.emplace_back();
        newEvent.type            = TYPE_GAMEPADREMOVED;
        newEvent.padStatus.which = newIndex;
    }

    /* this iterates the "active" joysticks only */
    for (size_t index = 0; index < MODULE()->GetJoystickCount(); index++)
    {
        Gamepad* gamepad = MODULE()->GetJoystickFromID(index);

        if (gamepad)
        {
            gamepad->UpdatePadState();

            /* handle button inputs */

            Gamepad::ButtonMapping button;

            const auto mappings = gamepad->GetButtonMapping();
            const auto entries  = mappings.GetEntries();

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
    }

    if (this->events.empty())
        return false;

    *event = this->events.front();
    this->events.pop_front();

    this->hysteresis = true;

    return true;
}
