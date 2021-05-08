#include "modules/audio/pool/pool.h"
#include "modules/audio/audio.h"

using namespace love;

void Pool::Sleep()
{
    LightEvent_Wait(&driver::Audrv::Instance().GetEvent());
}
