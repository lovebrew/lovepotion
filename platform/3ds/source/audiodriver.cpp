#include "common/runtime.h"
#include "audiodriver.h"

#include "common/exception.h"

using namespace love;

#define NDSP_LOAD_ERROR "Failed to load ndsp (Missing dspfirm.cdc?)."

void AudrenDriver::Initialize()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception(NDSP_LOAD_ERROR);
    else
        audioInit = true;
}

void AudrenDriver::Exit()
{
    if (audioInit)
        ndspExit();
}
