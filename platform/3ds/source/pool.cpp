#include "common/runtime.h"

#include "objects/source/source.h"
#include "modules/audio/pool/pool.h"

#include <unistd.h>

using namespace love;

void Pool::Sleep()
{
    // all we have to do
    svcSleepThread(SLEEP_TIME);
}
