#include "common/runtime.h"
#include "audiodriver.h"

#include "common/exception.h"

using namespace love;

void AudrenDriver::Initialize()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception("Failed to load ndsp. Please make sure your dspfirm.cdc has been dumped properly.");
    else
        audioInit = true;
}

void AudrenDriver::Exit()
{
    if (audioInit)
        ndspExit();
}
