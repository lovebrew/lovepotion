#include <utilities/driver/dsp_ext.hpp>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/drcvs.h>

using namespace love;

// clang-format off
static AXInitParams params = {
    .renderer = AX_INIT_RENDERER_48KHZ,
    .pipeline = AX_INIT_PIPELINE_SINGLE
};
// clang-format on

DSP<Console::CAFE>::DSP()
{
    if (!AXIsInit())
        AXInitWithParams(&params);

    this->initialized = true;
}

DSP<Console::CAFE>::~DSP()
{}

void DSP<Console::CAFE>::SetMasterVolume(AXDeviceType type, float volume)
{
    size_t channels = (type == AX_DEVICE_TYPE_TV) ? 6 : 4;

    for (size_t channel = 0; channel < channels; channel++)
        AXSetDeviceVolume(type, channel, volume);
}

float DSP<Console::CAFE>::GetMasterVolume(AXDeviceType type) const
{
    return 0.0f;
}
