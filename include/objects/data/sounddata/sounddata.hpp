#pragma once

#include <common/data.hpp>
#include <utilities/decoder/decoder.hpp>

namespace love
{
    class SoundData : public Data
    {
      public:
        static Type type;

        SoundData(Decoder* decoder);

        SoundData(int samples, int sampleRate, int bitDepth, int channels);

        SoundData(void* data, int samples, int sampleRate, int bitDepth, int channels);

        SoundData(const SoundData& other);

        virtual ~SoundData()
        {}

        SoundData* Clone() const;

        void* GetData() const;

        size_t GetSize() const;

        virtual int GetChannelCount() const;

        virtual int GetBitDepth() const;

        virtual int GetSampleRate() const;

        virtual int GetSampleCount() const;

        virtual float GetDuration() const;

        void SetSample(int index, float sample);

        void SetSample(int index, int channel, float sample);

        float GetSample(int index) const;

        float GetSample(int index, int channel) const;

      private:
        static constexpr int BUFFER_SIZE = 0x80000;

        void Load(int samples, int sampleRate, int bitDepth, int channels, void* newData = nullptr);

        std::vector<uint8_t> buffer;
        size_t size;

        int sampleRate;
        int bitDepth;
        int channels;
    };
} // namespace love
