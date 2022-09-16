#include <modules/joystickmodule_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

using namespace love;

JoystickModule<Console::CAFE>::JoystickModule()
{
    this->AddJoystick(0);

    for (size_t index = 0; index < this->AcquireCurrentJoystickIds().size(); index++)
        this->AddJoystick(index + 1);
}

JoystickModule<Console::CAFE>::~JoystickModule()
{}

std::vector<WPADChan> JoystickModule<Console::CAFE>::AcquireCurrentJoystickIds()
{
    std::vector<WPADChan> info {};

    for (size_t index = 0; index <= (size_t)WPAD_CHAN_3; index++)
    {
        WPADExtensionType extension;
        if (ResultCode(WPADProbe((WPADChan)index, &extension)).Success())
            info.push_back((WPADChan)index);
    }

    return info;
}

::Joystick* JoystickModule<Console::CAFE>::AddJoystick(int index)
{
    if (index < 0 || index >= (int)this->AcquireCurrentJoystickIds().size() + 1)
        return nullptr;

    guid::GamepadType type = guid::GAMEPAD_TYPE_WII_U_GAMEPAD;

    if (index != 0)
    {
        WPADExtensionType extension;
        WPADProbe((WPADChan)(index - 1), &extension);

        type = wpad::GetWPADType(extension);
    }

    std::string guid     = guid::GetDeviceGUID(type);
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
