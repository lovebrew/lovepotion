#include "common/runtime.h"

#include "objects/source/source.h"
#include "modules/audio/pool/pool.h"

#include "common/audiodriver.h"

#include <unistd.h>

using namespace love;

void Pool::Sleep()
{
    LightEvent_Wait(&AudrenDriver::ndspEvent);
}
