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
    struct AXWaveBuf
    {
        int bitDepth;
        int channels;
        uint8_t state;
        uint32_t endSamples;
        int16_t* data_pcm16;
        bool looping;

        AXWaveBuf* next;
    };

    struct AXChannel
    {
        uint8_t state;

        AXVoice* voice;
        AXWaveBuf* buffer;
    };

    template<>
    class DSP<Console::CAFE> : public DSP<Console::ALL>
    {
      public:
        enum PlayState : uint8_t
        {
            STATE_STOPPED = AX_VOICE_STATE_STOPPED,
            STATE_PLAYING = AX_VOICE_STATE_PLAYING,
            STATE_QUEUED,
            STATE_PAUSED,
            STATE_FINISHED
        };

        static DSP& Instance()
        {
            static DSP instance;
            return instance;
        }

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

        bool ChannelAddBuffer(size_t id, AXWaveBuf* buffer);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id);

        bool IsChannelPlaying(size_t id);

        void ChannelStop(size_t id);

        void SignalEvent();

      private:
        AXChannel axChannel[0x60];
        OSEvent event;
    };
} // namespace love
