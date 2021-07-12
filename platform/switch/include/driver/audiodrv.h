#pragma once

#include "common/driver/audiodrvc.h"
#include "modules/thread/types/mutex.h"

#include <switch.h>

#include <exception>

namespace love::driver
{
    class Audrv : public common::driver::Audrv
    {
      public:
        ~Audrv();

        static Audrv& Instance()
        {
            static Audrv instance;
            return instance;
        }

        bool ResetChannel(size_t channel, int channels, PcmFormat format, int sampleRate);

        void SetMixVolume(int mix, float volume);

        void SetChannelVolume(size_t channel, float volume);

        bool IsChannelPlaying(size_t channel);

        bool IsChannelPaused(size_t channel);

        bool AddWaveBuf(size_t channel, AudioDriverWaveBuf* waveBuf);

        void PauseChannel(size_t channel, bool pause);

        void StopChannel(size_t channel);

        u32 GetSampleOffset(size_t channel);

        void Update();

      private:
        Audrv();

        thread::MutexRef mutex;

        bool audioInitialized;
        bool channelReset;

        AudioDriver driver;
    };
} // namespace love::driver
