#include <modules/joystickmodule_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

using namespace love;

JoystickModule<Console::CAFE>::JoystickModule()
{
    this->AddJoystick(0);

    for (size_t index = 0; index < wpad::MAX_JOYSTICKS; index++)
        this->AddJoystick(index + 1);

    try
    {
        this->pool = new VibrationPool();
    }
    catch (love::Exception&)
    {
        throw;
    }

    this->thread = new PoolThread(this->pool);
    this->thread->Start();
}

JoystickModule<Console::CAFE>::~JoystickModule()
{
    this->thread->SetFinish();
    this->thread->Wait();

    delete this->thread;
    delete this->pool;
}

void JoystickModule<Console::CAFE>::AddVibration(::Vibration* vibration)
{
    this->pool->AddVibration(vibration);
}

Joystick<Console::Which>* JoystickModule<Console::CAFE>::AddJoystick(int index)
{
    if (index < 0 || index >= (int)wpad::MAX_JOYSTICKS + 1)
        return nullptr;

    bool reused            = false;
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

    std::string guid                   = guid::GetGamepadGUID(type);
    Joystick<Console::Which>* joystick = nullptr;

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
        joystick = new Joystick<Console::Which>((int)this->joysticks.size());
        this->joysticks.push_back(joystick);
    }

    this->RemoveJoystick(joystick);

    if (!joystick->Open(index))
        return nullptr;

    this->recentGUIDs[joystick->GetGUID()] = true;
    this->active.push_back(joystick);

    return joystick;
}
