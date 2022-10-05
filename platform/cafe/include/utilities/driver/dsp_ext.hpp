#pragma once

#include <coreinit/event.h>
#include <sndcore2/voice.h>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/driver/dsp.tcc>
#include <utilities/threads/threads.hpp>

#include <queue>

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
        uint8_t state;

        uint32_t endSamples;
        int16_t* data_pcm16;
        bool looping;
        uint16_t id;
    };

    struct AXChannel
    {
        uint8_t state;
        uint8_t channels;
        uint8_t bitDepth;
        uint16_t sampleRate;

        uint16_t currentId;
        uint16_t sequenceId;

        std::queue<AXWaveBuf*> queue;

        AXVoice* voice;
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

        void UpdateChannels();

        void ChannelStop(size_t id);

        void SignalEvent();

        // clang-format off
        static constexpr BidirectionalMap audioFormats = {
            0x08, AX_VOICE_FORMAT_LPCM8,
            0x10, AX_VOICE_FORMAT_LPCM16
        };
        // clang-format on

        static int8_t GetFormat(int bitDepth, int channels);

      private:
        love::mutex mutex;
        AXChannel axChannel[0x60];
        OSEvent event;
    };
} // namespace love
