#include "common/driver/hidrvc.h"

using namespace love::common::driver;

Hidrv::Hidrv() : hysteresis(false),
                 events(),
                 mutex(),
                 buttonStates()
{}

uint64_t Hidrv::GetButtonPressed()
{
    return this->buttonStates.pressed;
}

uint64_t Hidrv::GetButtonReleased()
{
    return this->buttonStates.released;
}

uint64_t Hidrv::GetButtonHeld()
{
    return this->buttonStates.held;
}

void Hidrv::SendFocus(bool focus)
{
    this->Lock();
        auto & event = this->events.emplace_back();

        event.type = TYPE_WINDOWEVENT;
        event.subType = focus ? TYPE_FOCUS_GAINED : TYPE_FOCUS_LOST;
    this->Unlock();
}

void Hidrv::SendLowMemory()
{
    this->Lock();
        auto & event = this->events.emplace_back();

        event.type = TYPE_LOWMEMORY;
    this->Unlock();
}

void Hidrv::SendQuit()
{
    this->Lock();
        auto & event = this->events.emplace_back();

        event.type = TYPE_QUIT;
    this->Unlock();
}

void Hidrv::SendResize(int width, int height)
{
    this->Lock();
        auto & event = this->events.emplace_back();

        event.type = TYPE_WINDOWEVENT;
        event.subType = TYPE_RESIZE;

        event.size.width  = width;
        event.size.height = height;
    this->Unlock();
}