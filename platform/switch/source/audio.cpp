#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/pool.h"

using namespace love;

Mutex g_audrvMutex;
AudioDriver g_AudioDriver;
std::atomic<bool> THREAD_RUN;

void AudioThreadUpdate(void * arg)
{
    auto pool = (std::vector<Source *> *)arg;

    while (THREAD_RUN)
    {
        auto iterator = pool->begin();

        while (iterator != pool->end())
        {
            if (!(*iterator)->Update())
            {
                (*iterator)->Release();
                iterator = pool->erase(iterator);
            }
            else
                iterator++;
        }

        mutexLock(&g_audrvMutex);
        audrvUpdate(&g_AudioDriver);
        mutexUnlock(&g_audrvMutex);

        audrenWaitFrame();
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

    AudioPool::AUDIO_POOL_BASE = memalign(AUDREN_MEMPOOL_ALIGNMENT, AudioPool::AUDIO_POOL_SIZE);

    Result ret = audrenInitialize(&config);

    this->audioInit = R_SUCCEEDED(ret);

    ret = audrvCreate(&g_AudioDriver, &config, 2);

    if (R_SUCCEEDED(ret))
        this->driverInit = true;
    else
        LOG("Failed to initialize audio driver");

    int mpid = audrvMemPoolAdd(&g_AudioDriver, AudioPool::AUDIO_POOL_BASE, AudioPool::AUDIO_POOL_SIZE);

    audrvMemPoolAttach(&g_AudioDriver, mpid);

    static const u8 sink_channels[] = { 0, 1 };

    audrvDeviceSinkAdd(&g_AudioDriver, AUDREN_DEFAULT_DEVICE_NAME, 2, sink_channels);

    audrvUpdate(&g_AudioDriver);

    audrenStartAudioRenderer();

    mutexInit(&g_audrvMutex);

    THREAD_RUN = true;

    u32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    ret = threadCreate(&this->poolThread, AudioThreadUpdate, (void *)&this->pool, NULL, 0x10000, priority - 1, 0);

    if (R_SUCCEEDED(ret))
        threadStart(&this->poolThread);
}

Audio::~Audio()
{
    THREAD_RUN = false;

    threadWaitForExit(&this->poolThread);
    threadClose(&this->poolThread);

    if (this->driverInit)
         audrvClose(&g_AudioDriver);

    if (this->audioInit)
         audrenExit();
}

void Audio::SetVolume(float volume)
{
    mutexLock(&g_audrvMutex);

    for (int i = 0; i < 2; i++)
        audrvMixSetVolume(&g_AudioDriver, i, volume);

    mutexUnlock(&g_audrvMutex);
}
