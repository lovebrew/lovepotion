#include <modules/joystickmodule_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <utilities/log/logfile.h>

using namespace love;

#define Module() Module::GetInstance<JoystickModule<Console::HAC>>(Module::M_JOYSTICK)

HID<Console::HAC>::HID() :
    touchState {},
    stateTouches {},
    oldStateTouches {},
    previousTouchCount(0),
    previousJoystickState {},
    previousGamepadTypes {}
{
    for (size_t index = 0; index < npad::MAX_JOYSTICKS; index++)
    {
        HidNpadIdType id = (HidNpadIdType)index;
        hidAcquireNpadStyleSetUpdateEventHandle(id, &this->statusEvents[index], true);
    }

    this->previousJoystickState = Module()->AcquireCurrentJoystickIds();
    this->previousGamepadTypes  = Module()->GetActiveStyleSets();
}

HID<Console::HAC>::~HID()
{
    for (auto event : this->statusEvents)
        eventClose(&event);
}

void HID<Console::HAC>::CheckFocus()
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
                std::pair<uint32_t, uint32_t> size;
                // ::deko3d::Instance().OnOperationMode(size);

                this->SendResize(size.first, size.second);

                break;
            }
            default:
                break;
        }
    }
}

bool HID<Console::HAC>::Poll(LOVE_Event* event)
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

    hidGetTouchScreenStates(&this->touchState, 1);
    int touchCount = this->touchState.count;

    if (touchCount > 0)
    {
        for (int id = 0; id < touchCount; id++)
        {
            auto& newEvent = this->events.emplace_back();
            newEvent.type  = TYPE_TOUCH;

            if (touchCount > this->previousTouchCount && id >= this->previousTouchCount)
            {
                this->stateTouches[id]    = this->touchState.touches[id];
                this->oldStateTouches[id] = this->stateTouches[id];

                newEvent.subType = SUBTYPE_TOUCHPRESS;
            }
            else
            {
                this->oldStateTouches[id] = this->stateTouches[id];
                this->stateTouches[id]    = this->touchState.touches[id];

                newEvent.subType = SUBTYPE_TOUCHMOVED;
            }

            newEvent.touchFinger.id = id;
            newEvent.touchFinger.x  = this->stateTouches[id].x;
            newEvent.touchFinger.y  = this->stateTouches[id].y;

            int32_t dx = this->stateTouches[id].x - this->oldStateTouches[id].x;
            int32_t dy = (int32_t)this->stateTouches[id].y - this->oldStateTouches[id].y;

            newEvent.touchFinger.dx       = dx;
            newEvent.touchFinger.dy       = dy;
            newEvent.touchFinger.pressure = 1.0f;

            if (newEvent.subType == SUBTYPE_TOUCHMOVED && !newEvent.touchFinger.dx &&
                !newEvent.touchFinger.dy)
            {
                this->events.pop_back();
                continue;
            }
        }
    }

    if (touchCount < this->previousTouchCount)
    {
        for (int id = 0; id < this->previousTouchCount; ++id)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_TOUCH;
            newEvent.subType = SUBTYPE_TOUCHRELEASE;

            newEvent.touchFinger.id       = id;
            newEvent.touchFinger.x        = this->stateTouches[id].x;
            newEvent.touchFinger.y        = this->stateTouches[id].y;
            newEvent.touchFinger.dx       = 0.0f;
            newEvent.touchFinger.dy       = 0.0f;
            newEvent.touchFinger.pressure = 0.0f;
        }
    }

    this->previousTouchCount = touchCount;

    if (Module())
    {
        for (auto event : this->statusEvents)
        {
            /* a controller was updated! */
            if (R_SUCCEEDED(eventWait(&event, 0)))
            {
                auto types = Module()->GetActiveStyleSets();

                for (size_t index = 0; index < this->previousGamepadTypes.size(); index++)
                {
                    if (std::find(types.begin(), types.end(), this->previousGamepadTypes[index]) ==
                        types.end())
                    {
                        // this->SendJoystickUpdated(index);
                    }
                }

                this->previousGamepadTypes = types;

                auto ids = Module()->AcquireCurrentJoystickIds();

                /* joystick removed */
                if (ids.size() < this->previousJoystickState.size())
                {
                    for (auto id : this->previousJoystickState)
                    {
                        if (std::find(ids.begin(), ids.end(), id) == ids.end())
                        {
                            this->SendJoystickStatus((size_t)id, false);
                            break;
                        }
                    }
                } /* joystick added */
                else if (ids.size() > this->previousJoystickState.size())
                    this->SendJoystickStatus((size_t)ids.back(), true);

                this->previousJoystickState = ids;
            }
        }

        for (size_t index = 0; index < Module()->GetJoystickCount(); index++)
        {
            auto* joystick = Module()->GetJoystick(index);

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
                    newEvent.padButton.id     = joystick->GetInstanceID();
                    newEvent.padButton.button = input.buttonNumber;
                }

                if (joystick->IsUp(input))
                {
                    auto& newEvent = this->events.emplace_back();

                    newEvent.type    = TYPE_GAMEPAD;
                    newEvent.subType = SUBTYPE_GAMEPADUP;

                    Joystick<>::GetConstant(input.button, newEvent.padButton.name);
                    newEvent.padButton.id     = joystick->GetInstanceID();
                    newEvent.padButton.button = input.buttonNumber;
                }

                /* handle trigger and stick inputs */
                for (size_t index = 0; index < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; index++)
                {
                    const auto axisEnum  = (Joystick<>::GamepadAxis)index;
                    const auto axisValue = joystick->GetAxis(index);

                    if (axisValue == this->stickValues[axisEnum])
                        continue;

                    auto& newEvent = this->events.emplace_back();

                    newEvent.type    = TYPE_GAMEPAD;
                    newEvent.subType = SUBTYPE_GAMEPADAXIS;

                    newEvent.padAxis.id = joystick->GetInstanceID();

                    const char* axis = nullptr;
                    Joystick<>::GetConstant(axisEnum, axis);

                    newEvent.padAxis.axis  = index;
                    newEvent.padAxis.value = axisValue;
                    newEvent.padAxis.name  = axis;

                    this->stickValues[axisEnum] = axisValue;
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
