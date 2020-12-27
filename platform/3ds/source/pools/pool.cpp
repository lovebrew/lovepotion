#include "modules/audio/pool/pool.h"

using namespace love;

void Pool::Sleep()
{
    LightEvent_Wait(&audrv.GetEvent());
}
