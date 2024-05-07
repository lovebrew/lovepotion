#include <modules/joystickmodule_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <modules/keyboard_ext.hpp>

#include <objects/gamepad.hpp>

#include <padscore/kpad.h>

#include <nn/swkbd.h>

#define Keyboard() (Module::GetInstance<Keyboard<Console::CAFE>>(Module::M_KEYBOARD))
#define Module()   (Module::GetInstance<JoystickModule<Console::CAFE>>(Module::M_JOYSTICK))

using namespace love;

HID<Console::CAFE>::HID() : previousTouch {}
{}

HID<Console::CAFE>::~HID()
{}

void HID<Console::CAFE>::CheckFocus()
{}

void HID<Console::CAFE>::CheckSoftwareKeyboard(VPADStatus vpadStatus)
{
    VPADGetTPCalibratedPoint(VPAD_CHAN_0, &vpadStatus.tpNormal, &vpadStatus.tpNormal);

    nn::swkbd::ControllerInfo controllerInfo {};
    controllerInfo.vpad = &vpadStatus;

    nn::swkbd::Calc(controllerInfo);

    if (nn::swkbd::IsNeedCalcSubThreadFont())
        nn::swkbd::CalcSubThreadFont();

    if (nn::swkbd::IsNeedCalcSubThreadPredict())
        nn::swkbd::CalcSubThreadPredict();

    bool isOkButtonPressed = nn::swkbd::IsDecideOkButton(nullptr);
    bool isCancelPressed   = nn::swkbd::IsDecideCancelButton(nullptr);

    if (isOkButtonPressed || isCancelPressed)
    {
        Keyboard()->HideKeyboard();

        if (isOkButtonPressed)
        {
            Keyboard()->Utf16toUtf8Text();
            this->SendTextInput(Keyboard()->GetText());
        }
    }
}

void HID<Console::CAFE>::_Poll()
{
    this->CheckFocus();

    if (!Module())
        return;

    auto status = ((Gamepad*)Module()->GetJoystickFromId(0))->GetVPADStatus();

    if (Keyboard()->IsShowing())
        this->CheckSoftwareKeyboard(status);

    auto tpNormal  = status.tpNormal;
    auto touchType = SUBTYPE_TOUCHPRESS;

    // TODO: find out how to fix initial touch issues
    // also why the fuck is this lagging so much
    if (tpNormal.touched)
    {
        VPADGetTPCalibratedPointEx(VPAD_CHAN_0, VPAD_TP_854X480, &tpNormal, &tpNormal);

        float x = tpNormal.x, y = tpNormal.y;
        float dx = 0, dy = 0;

        dx = (tpNormal.x - this->previousTouch.x);
        dy = (tpNormal.y - this->previousTouch.y);

        if (dx == 0 && dy == 0)
            touchType = SUBTYPE_TOUCHPRESS;
        else
            touchType = SUBTYPE_TOUCHMOVED;

        this->SendTouchEvent(touchType, 0, x, y, dx, dy, 1.0f);
        this->previousTouch = tpNormal;

        if (touchType == SUBTYPE_TOUCHMOVED && !dx && !dy)
            this->events.pop_back();
    }
    else
    {
        this->SendTouchEvent(SUBTYPE_TOUCHRELEASE, 0, this->previousTouch.x, this->previousTouch.y,
                             0, 0, 1.0f);
    }

    for (size_t index = 0; index < (size_t)Module()->GetJoystickCount(); index++)
    {
        auto* joystick = Module()->GetJoystickFromId(index);

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

            for (size_t index = 1; index < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; index++)
            {
                const auto axis  = (Joystick<>::GamepadAxis)index;
                const auto value = joystick->GetAxis(axis);

                this->SendGamepadAxis(joystick->GetID(), axis, index, value);
            }
        }
    }
}
