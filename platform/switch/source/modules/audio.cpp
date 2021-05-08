#include "modules/audio/audio.h"
#include "driver/audiodrv.h"

using namespace love;

void Audio::SetVolume(float volume)
{
    for (int mix = 0; mix < 2; mix++)
        driver::Audrv::Instance().SetMixVolume(mix, volume);
}
