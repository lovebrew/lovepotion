#pragma once

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/driver/dsp.tcc>

#include <3ds.h>

namespace love
{
    template<>
    class DSP<Console::CTR> : public DSP<Console::ALL>
    {
      public:
        static DSP& Instance()
        {
            static DSP instance;
            return instance;
        }

        ~DSP();

        void Initialize();

        void Update();

        void SetMasterVolume(float volume);

        float GetMasterVolume() const;

        bool ChannelReset(size_t id, int channels, int bitDepth, int sampleRate);

        void ChannelSetVolume(size_t id, float volume);

        float ChannelGetVolume(size_t id) const;

        bool ChannelAddBuffer(size_t id, ndspWaveBuf* buffer);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id) const;

        bool IsChannelPlaying(size_t id) const;

        void ChannelStop(size_t id);

        size_t ChannelGetSampleOffset(size_t id) const;

        // clang-format off
        static constexpr BidirectionalMap audioFormats = {
          0x08, NDSP_ENCODING_PCM8,
          0x10, NDSP_ENCODING_PCM16
        };

        static constexpr BidirectionalMap interpTypes = {
          0x01, NDSP_INTERP_LINEAR,
          0x02, NDSP_INTERP_POLYPHASE
        };
        // clang-format on

        static int8_t GetFormat(int bitDepth, int channels);

      private:
        LightEvent event;
    };
} // namespace love
