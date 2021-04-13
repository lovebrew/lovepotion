#include "modules/audio/pool/pool.h"
#include "modules/audio/audio.h"

using namespace love;

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))

void Pool::Sleep()
{
    if (!AudioModule())
        return;

    auto& tmp = AudioModule()->GetDriver();
    LightEvent_Wait(&tmp->GetEvent());
}
