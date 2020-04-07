#include "common/runtime.h"
#include "modules/audio/pool/pool.h"
#include "modules/audio/audio.h"

using namespace love;

Pool::_Update()
{
    mutexLock(&g_audrvMutex);
    audrvUpdate(&g_AudioDriver);
    mutexUnlock(&g_audrvMutex);

    audrenWaitFrame();
}
