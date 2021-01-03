#include "common/driver/hidrvc.h"

using namespace love::common::driver;

Hidrv::Hidrv() : hysteresis(false),
                 events(),
                 buttonPressed(0),
                 buttonReleased(0),
                 buttonHeld(0)
{}

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

void Hidrv::SendFocus(bool focus)
{
    this->LockFunction([&]() {
        auto & event = this->events.emplace_back();

        event.type = TYPE_WINDOWEVENT;
        event.subType = focus ? TYPE_FOCUS_GAINED : TYPE_FOCUS_LOST;
    });
}

void Hidrv::SendLowMemory()
{
    this->LockFunction([&]() {
        auto & event = this->events.emplace_back();

        event.type = TYPE_LOWMEMORY;
    });
}

void Hidrv::SendQuit()
{
    this->LockFunction([&]() {
        auto & event = this->events.emplace_back();

        event.type = TYPE_QUIT;
    });
}

void Hidrv::SendResize(int width, int height)
{
    this->LockFunction([&]() {
        auto & event = this->events.emplace_back();

        event.type = TYPE_WINDOWEVENT;
        event.subType = TYPE_RESIZE;

        event.size.width  = width;
        event.size.height = height;
    });
}