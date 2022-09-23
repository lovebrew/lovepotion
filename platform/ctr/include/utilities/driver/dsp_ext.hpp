#pragma once

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

        DSP();

        ~DSP();

        void SetMasterVolume(float volume);

        float GetMasterVolume() const;

        bool ChannelReset(size_t id, int channels, AudioFormat format, int bitDepth,
                          int sampleRate);

        void ChannelSetVolume(size_t id, float volume);

        float ChannelGetVolume(size_t id) const;

        bool ChannelAddBuffer(size_t id, ndspWaveBuf* buffer);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id) const;

        bool IsChannelPlaying(size_t id) const;

        void ChannelStop(size_t id);

        size_t ChannelGetSampleOffset(size_t id) const;

      private:
        LightEvent event;

        static void GetChannelFormat(AudioFormat format, int bitDepth, uint8_t& out);

        static bool GetInterpType(AudioFormat format, ndspInterpType& out);
    };
} // namespace love
