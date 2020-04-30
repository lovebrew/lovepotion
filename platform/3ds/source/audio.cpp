#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/exception.h"
#include "modules/audio/pool/pool.h"

using namespace love;

void Audio::_Initialize()
{
    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    this->poolThread = threadCreate(AudioThreadRunner, this->pool, AUDIO_THREAD_STACK_SIZE, priority - 1, -2, false);
}

void Audio::_Exit()
{
    threadJoin(this->poolThread, U64_MAX);
    threadFree(this->poolThread);
}

void Audio::SetVolume(float volume)
{
    ndspSetMasterVol(volume);
    this->volume = volume;
}
