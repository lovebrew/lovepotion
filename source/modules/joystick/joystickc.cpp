#include "modules/joystick/joystickc.h"

using namespace love::common;

Joystick::Joystick()
{
    this->activeCount = this->GetActiveControllerCount();
    for (size_t index = 0; index < activeCount; index++)
        this->AddGamepad(index);
}

Joystick::~Joystick()
{
    for (auto stick : this->gamepads)
    {
        stick->Close();
        stick->Release();
    }
}

love::Gamepad* Joystick::GetJoystickFromID(size_t index)
{
    for (auto stick : this->active)
    {
        if (stick->GetID() == index)
            return stick;
    }

    return nullptr;
}

size_t Joystick::GetJoystickCount() const
{
    return this->active.size();
}

love::Gamepad* Joystick::AddGamepad(size_t index)
{
    if (index < 0 || index >= this->activeCount)
        return nullptr;

    love::Gamepad* joystick = nullptr;
    bool reused             = false;

    for (auto stick : this->gamepads)
    {
        if (!stick->IsConnected())
        {
            joystick = stick;
            reused   = true;
            break;
        }
    }

    if (!joystick)
    {
        joystick = new love::Gamepad(gamepads.size());
        this->gamepads.push_back(joystick);
    }

    /*
    ** Make sure the Joystick object
    ** isn't in the active list already.
    */
    this->RemoveGamepad(joystick);

    if (!joystick->Open(index))
        return nullptr;

    for (auto activeStick : this->active)
    {
        if (joystick == activeStick)
        {
            joystick->Close();

            if (!reused)
            {
                this->gamepads.remove(joystick);
                joystick->Release();
            }

            return activeStick;
        }
    }

    this->active.push_back(joystick);

    return joystick;
}

void Joystick::RemoveGamepad(love::Gamepad* gamepad)
{
    if (!gamepad)
        return;

    auto iterator = std::find(this->active.begin(), this->active.end(), gamepad);

    if (iterator != this->active.end())
    {
        (*iterator)->Close();
        this->active.erase(iterator);
    }
}

int Joystick::CheckGamepadAdded()
{
    size_t active = this->GetActiveControllerCount();

    /* check that the active count updated */
    if (active > this->activeCount)
    {
        this->activeCount = active;
        return (active - 1);
    }

    /* nothing added */
    return -1;
}

int Joystick::CheckGamepadRemoved()
{
    size_t active = this->GetActiveControllerCount();

    /* check that the active count updated */
    if (active < this->activeCount)
    {
        this->activeCount = active;
        return active;
    }

    return -1;
}

int Joystick::GetIndex(const love::Gamepad* joystick)
{
    for (int i = 0; i < (int)this->active.size(); i++)
    {
        if (this->active[i] == joystick)
            return i;
    }

    // Joystick is not connected.
    return -1;
}
