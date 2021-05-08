#include "modules/audio/pool/pool.h"
#include "modules/audio/audio.h"

using namespace love;

void Pool::Sleep()
{
    driver::Audrv::Instance().Update();
    audrenWaitFrame();
}
