#include "common/driver/audiodrvc.h"

using namespace love::common::driver;

Audrv::Audrv() : initialized(false)
{}

const bool Audrv::IsInitialized()
{
    return this->initialized;
}