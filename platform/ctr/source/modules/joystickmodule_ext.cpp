#include <modules/joystickmodule_ext.hpp>

using namespace love;

JoystickModule<Console::CTR>::JoystickModule()
{
    for (int index = 0; index < this->GetCurrentJoystickCount(); index++)
        this->AddJoystick(index);
}

::Joystick* JoystickModule<Console::CTR>::AddJoystick(int index)
{
    if (index < 0 || index >= this->GetCurrentJoystickCount())
        return nullptr;

    std::string guid     = love::guid::GetGamepadGUID(guid::GAMEPAD_TYPE_NINTENDO_3DS);
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

    for (auto activeStick : this->active)
    {
        if (joystick->GetHandle() == activeStick->GetHandle())
        {
            joystick->Close();

            if (!reused)
            {
                this->joysticks.remove(joystick);
                joystick->Release();
            }

            return activeStick;
        }
    }

    this->recentGUIDs[joystick->GetGUID()] = true;
    this->active.push_back(joystick);

    return joystick;
}
