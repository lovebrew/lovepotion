#include <modules/joystickmodule_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <modules/keyboard_ext.hpp>

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
        nn::swkbd::DisappearInputForm();
        Keyboard()->HideKeyboard();

        if (isOkButtonPressed)
        {
            Keyboard()->Utf16toUtf8Text();
            this->SendTextInput(Keyboard()->GetText());
        }
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

            if (Keyboard()->IsShowing())
                this->CheckSoftwareKeyboard(joystick->GetVPADStatus());

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
                newEvent.touchFinger.dx       = (double)(dx / 0xFFFF);
                newEvent.touchFinger.dy       = (double)(dy / 0xFFFF);
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

            for (size_t axis = 0; axis < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; axis++)
            {
                const auto axisEnum  = (Joystick<>::GamepadAxis)axis;
                const auto axisValue = joystick->GetAxis(axis);

                if (axisValue == this->stickValues[0][axisEnum])
                    continue;

                auto& newEvent = this->events.emplace_back();

                newEvent.type    = TYPE_GAMEPAD;
                newEvent.subType = SUBTYPE_GAMEPADAXIS;

                newEvent.padAxis.id = joystick->GetInstanceID();

                const char* axisName = *Joystick<>::axisTypes.ReverseFind(axisEnum);

                newEvent.padAxis.axis  = axis;
                newEvent.padAxis.value = axisValue;
                newEvent.padAxis.name  = axisName;

                this->stickValues[0][axisEnum] = axisValue;
            }
        }
    }

    if (Module())
    {
        for (size_t index = 1; index < (size_t)Module()->GetJoystickCount(); index++)
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

                for (size_t axis = 0; axis < Joystick<>::GAMEPAD_AXIS_MAX_ENUM; axis++)
                {
                    const auto axisEnum  = (Joystick<>::GamepadAxis)axis;
                    const auto axisValue = joystick->GetAxis(axis);

                    if (axisValue == this->stickValues[index][axisEnum])
                        continue;

                    auto& newEvent = this->events.emplace_back();

                    newEvent.type    = TYPE_GAMEPAD;
                    newEvent.subType = SUBTYPE_GAMEPADAXIS;

                    newEvent.padAxis.id = joystick->GetInstanceID();

                    const char* axisName = *Joystick<>::axisTypes.ReverseFind(axisEnum);

                    newEvent.padAxis.axis  = axis;
                    newEvent.padAxis.value = axisValue;
                    newEvent.padAxis.name  = axisName;

                    this->stickValues[index][axisEnum] = axisValue;
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
