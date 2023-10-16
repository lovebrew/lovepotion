#pragma once

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/driver/dsp.tcc>
#include <utilities/threads/threads.hpp>

#include <switch.h>

namespace love
{
    template<>
    class DSP<Console::HAC> : public DSP<Console::ALL>
    {
      public:
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

        float ChannelGetVolume(size_t id) const;

        size_t ChannelGetSampleOffset(size_t id);

        bool ChannelAddBuffer(size_t id, AudioDriverWaveBuf* buffer);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id);

        bool IsChannelPlaying(size_t id);

        void ChannelStop(size_t id);

        // clang-format off
        static constexpr BidirectionalMap audioFormats = {
            0x08, PcmFormat_Int8,
            0x10, PcmFormat_Int16
        };
        // clang-format on

        static int8_t GetFormat(int bitDepth, int channels);

      private:
        love::mutex mutex;
        bool channelReset;
        AudioDriver driver;
    };
} // namespace love
