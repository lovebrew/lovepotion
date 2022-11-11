#include <utilities/pool/vibrations.hpp>

#include <modules/timer_ext.hpp>
#include <utilities/log/logfile.h>

using namespace love;

VibrationPool::VibrationPool()
{
    for (size_t index = 0; index < 8; index++)
        this->available.push(index);
}

VibrationPool::~VibrationPool()
{}

void VibrationPool::Update()
{
    std::unique_lock lock(this->mutex);
    std::vector<::Vibration*> release;

    for (const auto& iterator : this->vibrating)
    {
        if (Timer<Console::Which>::GetTime() > iterator.first->GetDuration())
            release.push_back(iterator.first);
        else
        {
            if (Console::Is(Console::CAFE))
            {
                float left, right = 0.0f;
                iterator.first->GetValues(left, right);
                iterator.first->SendValues(left, right);
            }
        }
    }

    for (auto* vibration : release)
        this->ReleaseVibration(vibration);
}

bool VibrationPool::FindVibration(::Vibration* vibration, size_t& id)
{
    const auto iterator = this->vibrating.find(vibration);

    if (iterator == this->vibrating.end())
        return false;

    id = iterator->second;
    return true;
}

bool VibrationPool::AddVibration(::Vibration* vibration)
{
    size_t index = 0;

    if (this->FindVibration(vibration, index))
        return true;

    if (this->available.empty())
        return false;

    index = this->available.front();
    this->available.pop();

    this->vibrating.insert(std::make_pair(vibration, index));

    return true;
}

bool VibrationPool::ReleaseVibration(::Vibration* vibration)
{
    size_t index = 0;

    if (this->FindVibration(vibration, index))
    {
        this->available.push(index);
        this->vibrating.erase(vibration);

        return true;
    }

    return false;
}
