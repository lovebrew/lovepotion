#include "driver/audiodrv.h"
#include "pools/audiopool.h"

using namespace love::driver;

Audrv::Audrv() : audioInitialized(false)
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

    AudioPool::AUDIO_POOL_BASE = memalign(AUDREN_MEMPOOL_ALIGNMENT, AudioPool::AUDIO_POOL_SIZE);

    Result res = audrenInitialize(&config);

    this->audioInitialized = R_SUCCEEDED(res);

    if (!this->audioInitialized)
        return;

    res = audrvCreate(&this->driver, &config, 2);

    if (R_SUCCEEDED(res))
        this->initialized = true;

    int mempoolID = audrvMemPoolAdd(&this->driver, AudioPool::AUDIO_POOL_BASE, AudioPool::AUDIO_POOL_SIZE);

    audrvMemPoolAttach(&this->driver, mempoolID);

    static const u8 sink_channels[] = { 0, 1 };

    audrvDeviceSinkAdd(&this->driver, AUDREN_DEFAULT_DEVICE_NAME, 2, sink_channels);

    audrvUpdate(&this->driver);

    audrenStartAudioRenderer();
}

Audrv::~Audrv()
{
    if (this->initialized)
         audrvClose(&this->driver);

    if (this->audioInitialized)
         audrenExit();
}

void Audrv::Update()
{
    thread::Lock lock(this->mutex);
    audrvUpdate(&this->driver);
}