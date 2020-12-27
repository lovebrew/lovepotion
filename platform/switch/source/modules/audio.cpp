#include "modules/audio/audio.h"
#include "driver/audrv.h"

using namespace love;

void Audio::SetVolume(float volume)
{
    audrv.LockFunction([volume](AudioDriver * driver) {
        for (int i = 0; i < 2; i++)
            audrvMixSetVolume(driver, i, volume);
    });
}
