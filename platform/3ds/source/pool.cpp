#include "modules/audio/pool/pool.h"
#include "common/audiodriver.h"

using namespace love;

void Pool::Sleep()
{
    this->Lock();
    LightEvent_Wait(&AudrenDriver::ndspEvent);
}
