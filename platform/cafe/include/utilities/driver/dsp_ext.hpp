#pragma once

#include <utilities/driver/dsp.tcc>
#include <utilities/threads/threads.hpp>

#include <sndcore2/voice.h>

namespace love
{
    template<>
    class DSP<Console::CAFE> : public DSP<Console::ALL>
    {
      public:
        static DSP& Instance()
        {
            static DSP instance;
            return instance;
        }

        DSP();

        ~DSP();

        void Update();

        void SetMasterVolume(AXDeviceType type, float volume);

        float GetMasterVolume(AXDeviceType type) const;

        bool ChannelReset(size_t id, int channels, int bitDepth, int sampleRate);

        void ChannelSetVolume(size_t id, float volume);

        float ChannelGetVolume(size_t id) const;

        size_t ChannelGetSampleOffset(size_t id);

        bool ChannelAddBuffer(size_t id, AXVoice* buffer);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id);

        bool IsChannelPlaying(size_t id);

        void ChannelStop(size_t id);

      private:
        love::mutex mutex;
    };
} // namespace love
