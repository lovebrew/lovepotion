#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/pool.h"

using namespace love;

Audio::Audio()
{
    static const AudioRendererConfig config =
    {
        .output_rate     = AudioRendererOutputRate_48kHz,
        .num_voices      = 24,
        .num_effects     = 0,
        .num_sinks       = 1,
        .num_mix_objs    = 1,
        .num_mix_buffers = 2,
    };

    Result ret = audrenInitialize(&config);

    if (R_SUCCEEDED(ret))
        ret = audrvCreate(&this->driver, &config, 2);

    if (R_SUCCEEDED(ret))
        this->driverInit = true;

    int mpid = audrvMemPoolAdd(&this->driver, AudioPool::AUDIO_POOL_BASE, AudioPool::AUDIO_POOL_SIZE);
    audrvMemPoolAttach(&this->driver, mpid);

    static const u8 sink_channels[] = { 0, 1 };
    audrvDeviceSinkAdd(&this->driver, AUDREN_DEFAULT_DEVICE_NAME, 2, sink_channels);

    audrvUpdate(&this->driver);
    audrenStartAudioRenderer();

    audrvVoiceInit(&this->driver, 0, 1, PcmFormat_Int16, 48000);
    audrvVoiceSetDestinationMix(&this->driver, 0, AUDREN_FINAL_MIX_ID);

    audrvVoiceSetMixFactor(&this->driver, 0, 1.0f, 0, 0);
    audrvVoiceSetMixFactor(&this->driver, 0, 1.0f, 0, 1);
}

AudioDriver & Audio::GetAudioDriver()
{
    return this->driver;
}

void Audio::UpdateAudioDriver()
{
    if (this->driverInit)
        audrvUpdate(&this->driver);
}

Audio::~Audio()
{
    audrvClose(&this->driver);
    audrenExit();
}

void Audio::SetVolume(float volume)
{
    for (int i = 0; i < 2; i++)
        audrvMixSetVolume(&this->driver, i, volume);
}
