#include "modules/audio/pool/pool.h"

using namespace love;

void Pool::Sleep()
{
    audrv.Update();
    audrenWaitFrame();
}
