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

            // if (graphics)
            //     graphics->SetActive(true);

            break;
        }
        case APTHOOK_ONSUSPEND:
        case APTHOOK_ONSLEEP:
        {
            driver.SendFocus(false);

            // if (graphics)
            //     graphics->SetActive(false);

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

void HID<Console::CTR>::_Poll()
{
    hidScanInput();

    const auto touchDown     = hidKeysDown();
    const auto touchHeld     = hidKeysHeld();
    const auto touchReleased = hidKeysUp();

    if (touchDown & KEY_TOUCH || touchHeld & KEY_TOUCH)
        hidTouchRead(&this->touchState.current);

    if (touchDown & KEY_TOUCH)
    {
        float x = this->touchState.current.px, y = this->touchState.current.py;
        this->SendTouchEvent(SUBTYPE_TOUCHPRESS, 0, x, y, 0.0f, 0.0f, 1.0f);

        this->touchState.previous = this->touchState.current;
    }
    else if (touchHeld & KEY_TOUCH)
    {
        float x = this->touchState.current.px, y = this->touchState.current.py;

        float dx = this->touchState.current.px - this->touchState.previous.px;
        float dy = this->touchState.current.py - this->touchState.previous.py;

        if (dx != 0.0f || dy != 0.0f)
            this->SendTouchEvent(SUBTYPE_TOUCHMOVED, 0, x, y, dx, dy, 1.0f);

        this->touchState.previous = this->touchState.current;
    }

    if (touchReleased & KEY_TOUCH)
    {
        float x = this->touchState.previous.px, y = this->touchState.previous.py;
        this->SendTouchEvent(SUBTYPE_TOUCHRELEASE, 0, x, y, 0.0f, 0.0f, 0.0f);
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

            if (joystick->IsSensorEnabled(sensor))
                this->SendJoystickSensorUpdated(0, sensor, joystick->GetSensorData(sensor));
        }

        if (joystick->IsDown(input))
            this->SendGamepadPress(true, joystick->GetID(), input.button, input.buttonNumber);

        if (joystick->IsUp(input))
            this->SendGamepadPress(false, joystick->GetID(), input.button, input.buttonNumber);

        /* handle trigger and stick inputs */
        for (size_t index = 0; index < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; index++)
        {
            const auto axis = (Joystick<>::GamepadAxis)index;

            if (joystick->IsAxisChanged(axis))
                this->SendGamepadAxis(0, axis, index, joystick->GetAxis(index));
        }
    }
}
