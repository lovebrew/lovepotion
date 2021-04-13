#include "driver/audiodrv.h"

using namespace love::driver;

void audioCallback(void* data)
{
    auto event = (LightEvent*)data;
    LightEvent_Signal(event);
}

Audrv::Audrv()
{
    this->initialized = R_SUCCEEDED(ndspInit());

    if (!this->initialized)
        return;

    LightEvent_Init(&this->ndspEvent, RESET_ONESHOT);
    ndspSetCallback(audioCallback, &this->ndspEvent);
}

Audrv::~Audrv()
{
    if (this->initialized)
        ndspExit();
}

LightEvent& Audrv::GetEvent()
{
    return this->ndspEvent;
}