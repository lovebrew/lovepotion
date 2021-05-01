#pragma once

#include "objects/decoder/decoder.h"
#include <limits>

namespace love
{
    class SoundData : public Data
    {
      public:
        static love::Type type;

        SoundData(Decoder* decoder);
        SoundData(int samples, int sampleRate, int bitDepth, int channels);
        SoundData(void* data, int samples, int sampleRate, int bitDepth, int channels);

        SoundData(const SoundData& other);

        ~SoundData();

        SoundData* Clone() const;

        void* GetData() const;

        size_t GetSize() const;

        int GetChannelCount() const;

        int GetBitDepth() const;

        int GetSampleRate() const;

        int GetSampleCount() const;

        float GetDuration() const;

        void SetSample(int i, float sample);

        void SetSample(int i, int channel, float sample);

        float GetSample(int i) const;

        float GetSample(int i, int channel) const;

      private:
        uint8_t* data;
        size_t size;

        int sampleRate;
        int bitDepth;
        int channels;

        void Load(int samples, int sampleRate, int bitDepth, int channels, void* newData = 0);
    };
} // namespace love
