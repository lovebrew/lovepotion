#include "pools/vibration.h"

#include "modules/timer/timer.h"

using namespace love;

bool VibrationPool::IsRunning()
{
    return this->running;
}

void VibrationPool::Finish()
{
    this->running = false;
}

bool VibrationPool::FindGamepad(Gamepad* gamepad)
{
    auto iterator = this->vibrating.find(gamepad);

    if (iterator == this->vibrating.end())
        return false;

    return true;
}

bool VibrationPool::ReleaseGamepad(Gamepad* gamepad, bool stop)
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

bool VibrationPool::AssignGamepad(Gamepad* gamepad, size_t id)
{
    this->vibrating.insert(std::make_pair(gamepad, id));
    gamepad->Retain();

    return true;
}

void VibrationPool::Update()
{
    thread::Lock lock(this->mutex);

    std::vector<Gamepad*> release;

    for (const auto& item : this->vibrating)
    {
        if (love::Timer::GetTime() > item.first->GetVibration().endTime)
            release.push_back(item.first);
    }

    for (Gamepad* gamepad : release)
        this->ReleaseGamepad(gamepad);
}

thread::Lock VibrationPool::Lock()
{
    return thread::Lock(this->mutex);
}