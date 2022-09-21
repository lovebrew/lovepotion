#include <modules/joystickmodule_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

using namespace love;

JoystickModule<Console::CAFE>::JoystickModule()
{
    this->AddJoystick(0);

    for (size_t index = 0; index < wpad::MAX_JOYSTICKS; index++)
        this->AddJoystick(index + 1);
}

JoystickModule<Console::CAFE>::~JoystickModule()
{}

::Joystick* JoystickModule<Console::CAFE>::AddJoystick(int index)
{
    if (index < 0 || index >= (int)wpad::MAX_JOYSTICKS + 1)
        return nullptr;

    guid::GamepadType type = guid::GAMEPAD_TYPE_WII_U_GAMEPAD;

    if (index != 0)
    {
        KPADStatus status {};
        KPADError error = KPAD_ERROR_OK;

        KPADReadEx((KPADChan)index, &status, 1, &error);

        if (error == KPAD_ERROR_OK && status.extensionType != 0xFF)
            type = wpad::GetWPADType((KPADExtensionType)status.extensionType);
        else
            return nullptr;
    }

    std::string guid     = guid::GetGamepadGUID(type);
    ::Joystick* joystick = nullptr;
    bool reused          = false;

    for (auto stick : this->joysticks)
    {
        if (!stick->IsConnected() && stick->GetGUID() == guid)
        {
            joystick = stick;
            reused   = true;
            break;
        }
    }

    if (!joystick)
    {
        joystick = new ::Joystick((int)this->joysticks.size());
        this->joysticks.push_back(joystick);
    }

    this->RemoveJoystick(joystick);

    if (!joystick->Open(index))
        return nullptr;

    this->recentGUIDs[joystick->GetGUID()] = true;
    this->active.push_back(joystick);

    return joystick;
}
