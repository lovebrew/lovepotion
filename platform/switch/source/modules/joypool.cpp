#include "modules/joystick/joypool.h"

#include "modules/timer/timer.h"

using namespace love;

bool JoystickPool::IsRunning()
{
    return this->running;
}

void JoystickPool::Finish()
{
    this->running = false;
}

bool JoystickPool::FindGamepad(Gamepad * gamepad)
{
    auto iterator = this->vibrating.find(gamepad);

    if (iterator == this->vibrating.end())
        return false;

    return true;
}

bool JoystickPool::ReleaseGamepad(Gamepad * gamepad, bool stop)
{
    if (this->FindGamepad(gamepad))
    {
        if (stop)
            gamepad->SetVibration();

        gamepad->Release();
        this->vibrating.erase(gamepad);

        return true;
    }

    return false;
}

bool JoystickPool::AssignGamepad(Gamepad * gamepad, size_t id)
{
    this->vibrating.insert(std::make_pair(gamepad, id));
    gamepad->Retain();

    return true;
}

void JoystickPool::Update()
{
    thread::Lock lock(this->mutex);

    std::vector<Gamepad *> release;

    for (const auto & item : this->vibrating)
    {
        if (love::Timer::GetTime() > item.first->GetVibration().endTime)
            release.push_back(item.first);
    }

    for (Gamepad * gamepad : release)
        this->ReleaseGamepad(gamepad);
}

thread::Lock JoystickPool::Lock()
{
    return thread::Lock(this->mutex);
}