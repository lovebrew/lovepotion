#include <modules/joystickmodule_ext.hpp>

#include <algorithm>
#include <memory>

#include <utilities/npad.hpp>

using namespace love;

JoystickModule<Console::HAC>::JoystickModule()
{
    for (int index = 0; index < this->GetCurrentJoystickCount(&this->activeIds); index++)
        this->AddJoystick(index);
}

JoystickModule<Console::HAC>::~JoystickModule()
{}

int JoystickModule<Console::HAC>::GetCurrentJoystickCount(std::vector<HidNpadIdType>* out)
{
    int total = 0;

    for (size_t index = 0; index < JoystickModule::MAX_JOYSTICKS; index++)
    {
        PadState state {};

        if (index == 0)
            padInitializeDefault(&state);
        else
            padInitialize(&state, (HidNpadIdType)index);

        padUpdate(&state);

        if (padIsConnected(&state))
        {
            total++;

            if (out != nullptr)
                out->push_back((HidNpadIdType)index);
        }
    }

    return total;
}

::Joystick* JoystickModule<Console::HAC>::AddJoystick(int index)
{
    if (index < 0 || index >= this->GetCurrentJoystickCount())
        return nullptr;

    PadState state {};

    padInitialize(&state, (HidNpadIdType)index);
    padUpdate(&state);

    auto styleTag = love::GetStyleTag(&state);

    guid::GamepadType type;
    love::GetConstant(styleTag, type);

    std::string guid     = love::guid::GetDeviceGUID(type);
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
