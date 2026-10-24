#include <modules/joystickmodule_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

#include <objects/gamepad.hpp>
#include <objects/procontroller.hpp>

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

static Joystick<Console::Which>* openByType(guid::GamepadType type, int index)
{
    switch (type)
    {
        case guid::GAMEPAD_TYPE_WII_U_GAMEPAD:
            return new Gamepad(index);
        case guid::GAMEPAD_TYPE_WII_PRO:
            return new ProController(index);
        default:
            return nullptr;
    }
}

Joystick<Console::Which>* JoystickModule<Console::CAFE>::AddJoystick(int index)
{
    if (index < 0 || index >= (int)wpad::MAX_JOYSTICKS + 1)
        return nullptr;

    bool reused            = false;
    guid::GamepadType type = guid::GAMEPAD_TYPE_WII_U_GAMEPAD;

    if (index != 0)
    {
        WPADExtensionType extension = (WPADExtensionType)0xFF;

        // WPADProbe uses channels 0..3; our joystick id starts at 1.
        if (WPADProbe((WPADChan)(index - 1), &extension) != KPAD_ERROR_OK)
            return nullptr;

        type = wpad::GetWPADType((KPADExtensionType)extension);

        if (type == guid::GAMEPAD_TYPE_UNKNOWN)
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
        joystick = openByType(type, (int)this->joysticks.size());

        // Unsupported extension (e.g. Wii Remote) must not become a joystick.
        if (!joystick)
            return nullptr;

        this->joysticks.push_back(joystick);
    }

    this->RemoveJoystick(joystick);

    if (!joystick->Open(index))
        return nullptr;

    this->recentGUIDs[joystick->GetGUID()] = true;
    this->active.push_back(joystick);

    return joystick;
}
