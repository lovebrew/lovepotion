#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/pool.h"
#include "wrap_audio.h"

using namespace love;

std::atomic<bool> THREAD_RUN;

void Threadfunction(void * arg)
{
    auto pool = (std::vector<Source *> *)arg;

    while (THREAD_RUN)
    {
        Wrap_Audio::_UpdateAudio();

        svcSleepThread(5000000);
    }
}

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
    {
        this->audioInit = true;
        ret = audrvCreate(&this->driver, &config, 2);
    }

    if (R_SUCCEEDED(ret))
        this->driverInit = true;

    AudioPool::AUDIO_POOL_BASE = memalign(AUDREN_MEMPOOL_ALIGNMENT, AudioPool::AUDIO_POOL_SIZE);

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

    THREAD_RUN = true;
    threadCreate(&this->poolThread, Threadfunction, (void *)&this->pool, NULL, 0x8000, 0x2C, -2);
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
    THREAD_RUN = false;

    threadWaitForExit(&this->poolThread);
    threadClose(&this->poolThread);

    if (this->driverInit)
        audrvClose(&this->driver);

    if (this->audioInit)
        audrenExit();
}

void Audio::SetVolume(float volume)
{
    for (int i = 0; i < 2; i++)
        audrvMixSetVolume(&this->driver, i, volume);
}
