#pragma once

#include "common/int.hpp"
#include "modules/filesystem/File.tcc"
#include "modules/sound/Decoder.hpp"

namespace love
{
    class SoundData : public Data
    {
      public:
        static Type type;

        SoundData(Decoder* decoder);

        SoundData(int samples, int sampleRate, int bitDepth, int channels);

        SoundData(const void* data, int samples, int sampleRate, int bitDepth, int channels);

        SoundData(const SoundData& other);

        virtual ~SoundData();

        SoundData* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

        virtual int getChannelCount() const;

        virtual int getBitDepth() const;

        virtual int getSampleRate() const;

        virtual int getSampleCount() const;

        virtual float getDuration() const;

        void setSample(int i, float sample);

        void setSample(int i, int channel, float sample);

        float getSample(int i) const;

        float getSample(int i, int channel) const;

        void copyFrom(const SoundData* source, int sourceStart, int count, int dstStart);

        SoundData* slice(int start, int length = -1) const;

      private:
        void load(int samples, int sampleRate, int bitDepth, int channels,
                  const void* newData = nullptr);

        std::vector<uint8_t> data;
        size_t size;

        int sampleRate;
        int bitDepth;
        int channels;
    };
} // namespace love
