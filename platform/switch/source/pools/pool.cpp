#include "modules/audio/pool/pool.h"
#include "modules/audio/audio.h"

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))

using namespace love;

void Pool::Sleep()
{
    AudioModule()->GetDriver()->Update();
    audrenWaitFrame();
}
