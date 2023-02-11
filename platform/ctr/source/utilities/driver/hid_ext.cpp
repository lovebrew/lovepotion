#include <modules/joystickmodule_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <modules/graphics_ext.hpp>

using namespace love;

#define Module() (Module::GetInstance<JoystickModule<Console::CTR>>(Module::M_JOYSTICK))
#define Sensor() (Module::GetInstance<Sensor<Console::CTR>>(Module::M_SENSOR))

static aptHookCookie s_aptHookCookie;

static void aptEventHook(const APT_HookType type, void* parameter)
{
    auto driver    = HID<Console::CTR>::Instance();
    auto* graphics = Module::GetInstance<Graphics<Console::CTR>>(Module::M_GRAPHICS);

    switch (type)
    {
        case APTHOOK_ONRESTORE:
        case APTHOOK_ONWAKEUP:
        {
            driver.SendFocus(true);

            if (graphics)
                graphics->SetActive(true);

            break;
        }
        case APTHOOK_ONSUSPEND:
        case APTHOOK_ONSLEEP:
        {
            driver.SendFocus(false);

            if (graphics)
                graphics->SetActive(false);

            break;
        }
        case APTHOOK_ONEXIT:
        {
            driver.SendQuit();
            break;
        }
        default:
            break;
    }
}

HID<Console::CTR>::HID() : touchState {}
{
    aptHook(&s_aptHookCookie, aptEventHook, nullptr);
}

HID<Console::CTR>::~HID()
{
    aptUnhook(&s_aptHookCookie);
}

bool HID<Console::CTR>::Poll(LOVE_Event* event)
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

    uint32_t touchDown     = hidKeysDown();
    uint32_t touchHeld     = hidKeysHeld();
    uint32_t touchReleased = hidKeysUp();

    if (touchDown & KEY_TOUCH || touchHeld & KEY_TOUCH)
        hidTouchRead(&this->touchState.current);

    if (!this->touchHeld && (touchDown & KEY_TOUCH))
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type    = TYPE_TOUCH;
        newEvent.subType = SUBTYPE_TOUCHPRESS;

        newEvent.touchFinger.id       = 0;
        newEvent.touchFinger.x        = this->touchState.current.px;
        newEvent.touchFinger.y        = this->touchState.current.py;
        newEvent.touchFinger.dx       = 0.0f;
        newEvent.touchFinger.dy       = 0.0f;
        newEvent.touchFinger.pressure = 1.0f;

        this->touchState.previous = this->touchState.current;
    }

    if (touchHeld & KEY_TOUCH)
    {
        float dx = this->touchState.previous.px - this->touchState.current.px;
        float dy = this->touchState.previous.py - this->touchState.current.py;

        if (dx != 0.0f || dy != 0.0f)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_TOUCH;
            newEvent.subType = SUBTYPE_TOUCHMOVED;

            newEvent.touchFinger.id       = 0;
            newEvent.touchFinger.x        = this->touchState.current.px;
            newEvent.touchFinger.y        = this->touchState.current.py;
            newEvent.touchFinger.dx       = dx;
            newEvent.touchFinger.dy       = dy;
            newEvent.touchFinger.pressure = 1.0f;

            this->touchHeld = true;
        }
    }

    if (touchReleased & KEY_TOUCH)
    {
        auto& newEvent            = this->events.emplace_back();
        this->touchState.previous = this->touchState.current;

        newEvent.type    = TYPE_TOUCH;
        newEvent.subType = SUBTYPE_TOUCHRELEASE;

        newEvent.touchFinger.id       = 0;
        newEvent.touchFinger.x        = this->touchState.previous.px;
        newEvent.touchFinger.y        = this->touchState.previous.py;
        newEvent.touchFinger.dx       = 0.0f;
        newEvent.touchFinger.dy       = 0.0f;
        newEvent.touchFinger.pressure = 0.0f;

        if (this->touchHeld)
            this->touchHeld = false;
    }

    Joystick<Console::Which>* joystick = nullptr;

    if (Module())
        joystick = Module()->GetJoystickFromId(0);

    if (joystick)
    {
        joystick->Update();
        Joystick<>::JoystickInput input {};

        for (int index = 0; index < Sensor::SENSOR_MAX_ENUM; index++)
        {
            const auto sensor = (Sensor::SensorType)index;
            this->SendJoystickSensorUpdated(0, sensor, joystick->GetSensorData(sensor));
        }

        if (joystick->IsDown(input))
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_GAMEPAD;
            newEvent.subType = SUBTYPE_GAMEPADDOWN;

            newEvent.padButton.name   = *Joystick<>::buttonTypes.ReverseFind(input.button);
            newEvent.padButton.id     = joystick->GetID();
            newEvent.padButton.button = input.buttonNumber;
        }

        if (joystick->IsUp(input))
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type    = TYPE_GAMEPAD;
            newEvent.subType = SUBTYPE_GAMEPADUP;

            newEvent.padButton.name   = *Joystick<>::buttonTypes.ReverseFind(input.button);
            newEvent.padButton.id     = joystick->GetID();
            newEvent.padButton.button = input.buttonNumber;
        }

        /* handle trigger and stick inputs */
        for (size_t index = 0; index < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; index++)
        {
            const auto axisEnum = (Joystick<>::GamepadAxis)index;

            if (joystick->IsAxisChanged(axisEnum))
            {
                auto& newEvent = this->events.emplace_back();

                newEvent.type    = TYPE_GAMEPAD;
                newEvent.subType = SUBTYPE_GAMEPADAXIS;

                newEvent.padAxis.id = joystick->GetID();

                const char* axis = *Joystick<>::axisTypes.ReverseFind(axisEnum);

                newEvent.padAxis.axis  = index;
                newEvent.padAxis.value = joystick->GetAxis(index);
                newEvent.padAxis.name  = axis;
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
