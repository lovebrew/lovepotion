#include "modules/audio/audio.h"

#include "audiodriver.h"

using namespace love;

void Audio::SetVolume(float volume)
{
    AudrenDriver::LockFunction([volume](AudioDriver * driver) {
        for (int i = 0; i < 2; i++)
            audrvMixSetVolume(driver, i, volume);
    });
}
