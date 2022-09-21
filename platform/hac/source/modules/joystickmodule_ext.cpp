#include <modules/joystickmodule_ext.hpp>

#include <algorithm>
#include <memory>

#include <utilities/npad.hpp>

using namespace love;

JoystickModule<Console::HAC>::JoystickModule()
{
    for (int index = 0; index < (int)this->AcquireCurrentJoystickIds().size(); index++)
        this->AddJoystick(index);
}

JoystickModule<Console::HAC>::~JoystickModule()
{}

std::vector<HidNpadIdType> JoystickModule<Console::HAC>::AcquireCurrentJoystickIds()
{
    std::vector<HidNpadIdType> info {};

    for (size_t index = 0; index < npad::MAX_JOYSTICKS; index++)
    {
        PadState state {};

        if (index == 0)
            padInitializeDefault(&state);
        else
            padInitialize(&state, (HidNpadIdType)index);

        padUpdate(&state);

        if (padIsConnected(&state))
            info.push_back((HidNpadIdType)index);
    }

    return info;
}

std::vector<guid::GamepadType> JoystickModule<Console::HAC>::GetActiveStyleSets()
{
    std::vector<guid::GamepadType> info {};
    HidNpadStyleTag tag;

    for (size_t index = 0; index < npad::MAX_JOYSTICKS; index++)
    {
        PadState state {};

        if (index == 0)
            padInitializeDefault(&state);
        else
            padInitialize(&state, (HidNpadIdType)index);

        padUpdate(&state);

        if (padIsConnected(&state))
        {
            tag = npad::GetStyleTag(&state);

            guid::GamepadType type;
            if (npad::GetConstant(tag, type))
                info.push_back(type);
        }
    }

    return info;
}

::Joystick* JoystickModule<Console::HAC>::AddJoystick(int index)
{
    if (index < 0 || index >= (int)this->AcquireCurrentJoystickIds().size())
        return nullptr;

    PadState state {};

    padInitialize(&state, (HidNpadIdType)index);
    padUpdate(&state);

    auto styleTag = npad::GetStyleTag(&state);

    guid::GamepadType type;
    npad::GetConstant(styleTag, type);

    std::string guid     = love::guid::GetGamepadGUID(type);
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

    // for (auto activeStick : this->active)
    // {
    //     if (joystick->GetHandle() == activeStick->GetHandle())
    //     {
    //         joystick->Close();

    //         if (!reused)
    //         {
    //             this->joysticks.remove(joystick);
    //             joystick->Release();
    //         }

    //         return activeStick;
    //     }
    // }

    this->recentGUIDs[joystick->GetGUID()] = true;
    this->active.push_back(joystick);

    return joystick;
}
