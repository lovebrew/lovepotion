#pragma once

#include <utilities/driver/dsp.tcc>

#include <3ds.h>

namespace love
{
    template<>
    class DSP<Console::CTR> : public DSP<Console::ALL>
    {
      public:
        class DataBuffer : public Object
        {
          public:
            DataBuffer(void* data, size_t size) : size(size)
            {
                this->buffer = (int16_t*)linearAlloc(size);
                std::copy_n((int16_t*)data, size, this->buffer);
            }

            ~DataBuffer()
            {
                linearFree(this->buffer);
            }

            inline int16_t* GetBuffer() const
            {
                return this->buffer;
            }

            inline size_t GetSize() const
            {
                return this->size;
            }

          private:
            int16_t* buffer;
            size_t size;
        };

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

      private:
        LightEvent event;

        static void GetChannelFormat(AudioFormat format, int bitDepth, uint8_t& out);

        static bool GetInterpType(AudioFormat format, ndspInterpType& out);
    };
} // namespace love
