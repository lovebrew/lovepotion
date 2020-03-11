#include "common/runtime.h"
#include "modules/joystick/joystick.h"

using namespace love;

Joystick::Joystick()
{
    for (size_t index = 0; index < MAX_GAMEPADS; index++)
        this->AddGamepad(index);
}

Joystick::~Joystick()
{
    for (auto stick : this->gamepads)
        stick->Release();
}

size_t Joystick::GetJoystickCount()
{
    return this->gamepads.size();
}

Gamepad * Joystick::GetJoystickFromID(size_t index)
{
    for (auto stick : this->gamepads)
    {
        if (stick->GetID() == index)
            return stick;
    }

    return nullptr;
}

Gamepad * Joystick::AddGamepad(size_t index)
{
    Gamepad * joystick = new Gamepad(index);
    this->gamepads.push_back(joystick);

    return joystick;
}
