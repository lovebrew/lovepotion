#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/exception.h"

using namespace love;

void Audio::UpdateAudioDriver()
{}

Audio::Audio()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception("Failed to load ndsp. Please make sure your dspfirm.cdc has been dumped properly.");
}

Audio::~Audio()
{
    ndspExit();
}

void Audio::SetVolume(float volume)
{
    ndspSetMasterVol(volume);
    this->volume = volume;
}
