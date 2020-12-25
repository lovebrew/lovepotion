#include "modules/audio/audio.h"

#include "common/exception.h"
#include "modules/audio/pool/pool.h"

using namespace love;

void Audio::SetVolume(float volume)
{
    ndspSetMasterVol(volume);
    this->volume = volume;
}
