#pragma once

#include <utilities/driver/dsp.tcc>
#include <utilities/threads/threads.hpp>

#include <coreinit/event.h>
#include <sndcore2/voice.h>

#include <map>

extern "C"
{
    /* todo - remove when wut supports these */
    void AXSetMasterVolume(uint32_t volume);
    int16_t AXGetMasterVolume();
}

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

        struct AXWaveBuf
        {
            uint8_t* data_pcm8;
            uint16_t* data_pcm16;
        };

        DSP();

        ~DSP();

        void Initialize();

        void Update();

        void SetMasterVolume(float volume);

        float GetMasterVolume() const;

        bool ChannelReset(size_t id, int channels, int bitDepth, int sampleRate);

        void ChannelSetVolume(size_t id, float volume);

        float ChannelGetVolume(size_t id);

        size_t ChannelGetSampleOffset(size_t id);

        bool ChannelAddBuffer(size_t id);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id);

        bool IsChannelPlaying(size_t id);

        void ChannelStop(size_t id);

        OSEvent GetEvent();

      private:
        love::mutex mutex;
        std::map<size_t, AXVoice*> channels;
        OSEvent event;

        AXVoice* FindVoice(size_t channel);
    };
} // namespace love
