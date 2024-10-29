#include <modules/graphics_ext.hpp>
#include <modules/joystickmodule_ext.hpp>

#include <utilities/driver/hid_ext.hpp>

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

        auto* graphics = Module::GetInstance<Graphics<Console::HAC>>(Module::M_GRAPHICS);

        switch (message)
        {
            case AppletMessage_FocusStateChanged:
            {

                bool oldFocus          = focused;
                AppletFocusState state = appletGetFocusState();

                focused = (state == AppletFocusState_InFocus);

                this->SendFocus(focused);

                if (graphics)
                    graphics->SetActive(focused);

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

void HID<Console::HAC>::_Poll()
{
    this->CheckFocus();

    hidGetTouchScreenStates(&this->touchState, 1);
    int touchCount = this->touchState.count;

    if (touchCount > 0)
    {
        for (int id = 0; id < touchCount; id++)
        {
            auto touchType = SUBTYPE_TOUCHPRESS;

            if (touchCount > this->previousTouchCount && id >= this->previousTouchCount)
            {
                this->stateTouches[id]    = this->touchState.touches[id];
                this->oldStateTouches[id] = this->stateTouches[id];

                touchType = SUBTYPE_TOUCHPRESS;
            }
            else
            {
                this->oldStateTouches[id] = this->stateTouches[id];
                this->stateTouches[id]    = this->touchState.touches[id];

                touchType = SUBTYPE_TOUCHMOVED;
            }

            float x = this->stateTouches[id].x, y = this->stateTouches[id].y;

            int32_t dx = this->stateTouches[id].x - this->oldStateTouches[id].x;
            int32_t dy = (int32_t)this->stateTouches[id].y - this->oldStateTouches[id].y;

            this->SendTouchEvent(touchType, id, x, y, dx, dy, 1.0f);

            if (touchType == SUBTYPE_TOUCHMOVED && !dx && !dy)
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
            float x = this->stateTouches[id].x, y = this->stateTouches[id].y;
            this->SendTouchEvent(SUBTYPE_TOUCHRELEASE, id, x, y, 0.0f, 0.0f, 0.0f);
        }
    }

    this->previousTouchCount = touchCount;

    if (!Module())
        return;

    for (auto event : this->statusEvents)
    {
        /* a controller was updated! */
        if (R_SUCCEEDED(eventWait(&event, 0)))
        {
            auto types = Module()->GetActiveStyleSets();

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

    const auto joystickCount = Module()->GetJoystickCount();

    for (size_t index = 0; index < joystickCount; index++)
    {
        auto* joystick = Module()->GetJoystick(index);

        if (joystick)
        {
            joystick->Update();
            Joystick<>::JoystickInput input {};

            for (int index = 0; index < Sensor::SENSOR_MAX_ENUM; index++)
            {
                const auto sensor = (Sensor::SensorType)index;

                if (joystick->IsSensorEnabled(sensor))
                    this->SendJoystickSensorUpdated(index, sensor, joystick->GetSensorData(sensor));
            }

            if (joystick->IsDown(input))
                this->SendGamepadPress(true, joystick->GetID(), input.button, input.buttonNumber);

            if (joystick->IsUp(input))
                this->SendGamepadPress(false, joystick->GetID(), input.button, input.buttonNumber);

            /* handle trigger and stick inputs */
            for (size_t index = 0; index < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; index++)
            {
                const auto axis  = (Joystick<>::GamepadAxis)index;
                const auto value = joystick->GetAxis(index);

                if (joystick->IsAxisChanged(axis))
                    this->SendGamepadAxis(joystick->GetID(), axis, index, value);
            }
        }
    }
}
