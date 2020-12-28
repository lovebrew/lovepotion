#include "common/driver/hidrvc.h"

using namespace love::common::driver;

uint64_t Hidrv::GetButtonPressed()
{
    return this->buttonPressed;
}

uint64_t Hidrv::GetButtonReleased()
{
    return this->buttonReleased;
}

uint64_t Hidrv::GetButtonHeld()
{
    return this->buttonHeld;
}

void Hidrv::SendFocus(const bool focus)
{
    love::thread::Lock lock(this->mutex);

    auto & event = this->events.emplace_back();

    event.type = TYPE_WINDOWEVENT;
    event.subType = focus ? TYPE_FOCUS_GAINED : TYPE_FOCUS_LOST;
}

void Hidrv::SendLowMemory()
{
    love::thread::Lock lock(this->mutex);

    auto & event = this->events.emplace_back();

    event.type = TYPE_LOWMEMORY;
}

void Hidrv::SendQuit()
{
    love::thread::Lock lock(this->mutex);

    auto & event = this->events.emplace_back();

    event.type = TYPE_QUIT;
}

void Hidrv::SendResize(const int width, const int height)
{
    love::thread::Lock lock(this->mutex);

    auto & event = this->events.emplace_back();

    event.type = TYPE_WINDOWEVENT;
    event.subType = TYPE_RESIZE;

    event.size.width  = width;
    event.size.height = height;
}