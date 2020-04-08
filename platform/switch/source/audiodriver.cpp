#include "common/runtime.h"
#include "audiodriver.h"

#include "common/audiopool.h"

using namespace love;

love::thread::MutexRef AudrenDriver::mutex;
AudioDriver AudrenDriver::driver;

void AudrenDriver::Initialize()
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

    Result ret = audrenInitialize(&config);

    audioInit = R_SUCCEEDED(ret);

    ret = audrvCreate(&driver, &config, 2);

    if (R_SUCCEEDED(ret))
        driverInit = true;

    int mpid = audrvMemPoolAdd(&driver, AudioPool::AUDIO_POOL_BASE, AudioPool::AUDIO_POOL_SIZE);

    audrvMemPoolAttach(&driver, mpid);

    static const u8 sink_channels[] = { 0, 1 };

    audrvDeviceSinkAdd(&driver, AUDREN_DEFAULT_DEVICE_NAME, 2, sink_channels);

    audrvUpdate(&driver);

    audrenStartAudioRenderer();
}

void AudrenDriver::Update()
{
    thread::Lock lock(mutex);
    audrvUpdate(&driver);
}

void AudrenDriver::Exit()
{
    if (driverInit)
         audrvClose(&driver);

    if (audioInit)
         audrenExit();
}
