#include "modules/audio/pool/pool.h"

#include "audiodriver.h"

using namespace love;

void Pool::Sleep()
{
    AudrenDriver::Update();
    audrenWaitFrame();
}
