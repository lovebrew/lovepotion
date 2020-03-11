#pragma once

#include "objects/decoder/decoder.h"

namespace love
{
    class SoundData : public Data
    {
        public:
            static love::Type type;

            SoundData(Decoder * decoder);
            SoundData(const SoundData & other);

            ~SoundData();

            SoundData * Clone() const;

            void * GetData() const;

            size_t GetSize() const;

            int GetChannelCount() const;

            int GetBitDepth() const;

            int GetSampleRate() const;

            int GetSampleCount() const;

            float GetDuration() const;

        private:
            uint8_t * data;
            size_t size;

            int sampleRate;
            int bitDepth;
            int channels;

            void Load(int samples, int sampleRate, int bitDepth, int channels, void * newData);
    };
}
