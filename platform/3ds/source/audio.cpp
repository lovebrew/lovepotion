#include "common/runtime.h"
#include "modules/audio/audio.h"

using namespace love;

void Audio::UpdateAudioDriver()
{}

Audio::Audio()
{
    ndspInit();
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
