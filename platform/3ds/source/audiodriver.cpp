#include "common/runtime.h"
#include "audiodriver.h"

#include "common/exception.h"

using namespace love;

#define NDSP_LOAD_ERROR "Failed to load ndsp (Missing dspfirm.cdc?)."

LightEvent AudrenDriver::ndspEvent;

void AudrenDriver::Initialize()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception(NDSP_LOAD_ERROR);
    else
        audioInit = true;

    LightEvent_Init(&AudrenDriver::ndspEvent, ResetType::RESET_STICKY);

    ndspSetCallback(AudrenDriver::Callback, nullptr);
}

void AudrenDriver::Callback(void * data)
{
    LightEvent_Signal(&AudrenDriver::ndspEvent);
}

void AudrenDriver::Exit()
{
    if (audioInit)
        ndspExit();
}
