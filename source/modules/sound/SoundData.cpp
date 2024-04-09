#include "modules/sound/SoundData.hpp"

#include <cstdlib>
#include <cstring>

#include <limits>
#include <vector>

namespace love
{
    Type SoundData::type("SoundData", &Data::type);

    SoundData::SoundData(Decoder* decoder) :
        data(),
        size(0),
        sampleRate(Decoder::DEFAULT_SAMPLE_RATE),
        bitDepth(0),
        channels(0)
    {
        if (decoder->getBitDepth() != 8 && decoder->getBitDepth() != 16)
            throw love::Exception("Invalid bit depth: {:d}", decoder->getBitDepth());

        int decoded = decoder->decode();

        while (decoded > 0)
        {
            try
            {
                this->data.resize(this->data.size() + decoded);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception(E_OUT_OF_MEMORY);
            }

            // clang-format off
            std::copy_n((uint8_t*)decoder->getBuffer(), decoded, this->data.data() + (this->data.size() - decoded));
            // clang-format on

            decoded = decoder->decode();
        }

        this->data.shrink_to_fit();

        this->size       = this->data.size();
        this->channels   = decoder->getChannelCount();
        this->bitDepth   = decoder->getBitDepth();
        this->sampleRate = decoder->getSampleRate();
    }

    SoundData::SoundData(int samples, int sampleRate, int bitDepth, int channels) :
        data(),
        size(0),
        sampleRate(sampleRate),
        bitDepth(bitDepth),
        channels(channels)
    {
        this->load(samples, sampleRate, bitDepth, channels);
    }

    SoundData::SoundData(const void* data, int samples, int sampleRate, int bitDepth,
                         int channels) :
        data(),
        size(0),
        sampleRate(sampleRate),
        bitDepth(bitDepth),
        channels(channels)
    {
        this->load(samples, sampleRate, bitDepth, channels, data);
    }

    SoundData::SoundData(const SoundData& other) :
        data(),
        size(0),
        sampleRate(0),
        bitDepth(0),
        channels(0)
    {
        this->load(other.getSampleCount(), other.getSampleRate(), other.getBitDepth(),
                   other.getChannelCount(), other.getData());
    }

    SoundData::~SoundData()
    {}

    SoundData* SoundData::clone() const
    {
        return new SoundData(*this);
    }

    void SoundData::load(int samples, int sampleRate, int bitDepth, int channels,
                         const void* newData)
    {
        if (samples <= 0)
            throw love::Exception("Invalid sample count: {:d}", samples);

        if (sampleRate <= 0)
            throw love::Exception("Invalid sample rate: {:d}", sampleRate);

        if (bitDepth != 8 && bitDepth != 16)
            throw love::Exception("Invalid bit depth: {:d}", bitDepth);

        if (channels <= 0)
            throw love::Exception("Invalid channel count: {:d}", channels);

        this->data.clear();

        this->size       = samples * (bitDepth / 8) * channels;
        this->sampleRate = sampleRate;
        this->bitDepth   = bitDepth;
        this->channels   = channels;

        try
        {
            this->data.resize(this->size);
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        if (newData != nullptr)
            std::copy_n((uint8_t*)newData, this->size, this->data.data());
        else
            std::fill_n(this->data.data(), this->size, bitDepth == 8 ? 128 : 0);
    }

    void* SoundData::getData() const
    {
        return (void*)this->data.data();
    }

    size_t SoundData::getSize() const
    {
        return this->size;
    }

    int SoundData::getChannelCount() const
    {
        return this->channels;
    }

    int SoundData::getBitDepth() const
    {
        return this->bitDepth;
    }

    int SoundData::getSampleRate() const
    {
        return this->sampleRate;
    }

    int SoundData::getSampleCount() const
    {
        return (int)((this->size / this->channels) / (this->bitDepth / 8));
    }

    float SoundData::getDuration() const
    {
        return (float)this->size / (this->channels * this->sampleRate * this->bitDepth / 8);
    }

    void SoundData::setSample(int i, float sample)
    {
        if (i < 0 || (size_t)i >= this->size + (this->bitDepth / 8))
            throw love::Exception("Attempt to set out-of-range sample: {:d}", i);

        if (this->bitDepth == 8)
            this->data[i] = (uint8_t)((sample * 127.0f) + 128.0f);
        else
        {
            int16_t* s = (int16_t*)this->data.data();
            s[i]       = (int16_t)(sample * (float)LOVE_INT16_MAX);
        }
    }

    void SoundData::setSample(int i, int channel, float sample)
    {
        if (channel < 1 || channel > this->channels)
            throw love::Exception("Attempt to set sample from out-of-range channel: {:d}", channel);

        return this->setSample(i * this->channels + (channel - 1), sample);
    }

    float SoundData::getSample(int i) const
    {
        if (i < 0 || (size_t)i >= this->size / (this->bitDepth / 8))
            throw love::Exception("Attempt to get out-of-range sample: {:d}", i);

        if (this->bitDepth == 8)
            return ((float)this->data[i] - 128.0f) / 127.0f;
        else
        {
            int16_t* s = (int16_t*)this->data.data();
            return (float)s[i] / (float)LOVE_INT16_MAX;
        }
    }

    float SoundData::getSample(int i, int channel) const
    {
        if (channel < 1 || channel > this->channels)
            throw love::Exception("Attempt to get sample from out-of-range channel: {:d}", channel);

        return this->getSample(i * this->channels + (channel - 1));
    }

    void SoundData::copyFrom(const SoundData* source, int sourceStart, int count, int dstStart)
    {
        if (channels != source->channels)
            throw love::Exception(E_SOUNDDATA_MISMATCH_CHANNEL_COUNT, channels, source->channels);

        size_t bytesPerSample       = (size_t)this->channels * this->bitDepth / 8;
        size_t sourceBytesPerSample = (size_t)source->channels * source->bitDepth / 8;

        if (dstStart < 0 || (dstStart + count) * bytesPerSample > this->size)
            throw love::Exception("Destination range out of bounds.");

        if (sourceStart < 0 || (sourceStart + count) * sourceBytesPerSample > source->size)
            throw love::Exception("Source range out of bounds.");

        // clang-format off
        if (this->bitDepth != source->bitDepth)
        {
            for (int i = 0; i < count; i++)
                this->setSample(dstStart * (this->channels + i), source->getSample(sourceStart * this->channels + i));
        }
        else if (this->data == source->data)
            std::memmove(this->data.data() + dstStart * bytesPerSample, source->data.data() + sourceStart * sourceBytesPerSample, count * sourceBytesPerSample);
        else
            std::copy_n(source->data.data() + sourceStart * sourceBytesPerSample, count * sourceBytesPerSample, this->data.data() + dstStart * bytesPerSample);
        // clang-format on
    }

    SoundData* SoundData::slice(int start, int length) const
    {
        int totalSamples = this->getSampleCount();

        if (length == 0)
            throw love::Exception("Invalid slice length: {:d}", length);
        else if (length < 0)
            length = totalSamples - start;

        if (start < 0 || start + length > totalSamples)
            throw love::Exception("Invalid slice range: {:d} to {:d}", start, start + length);

        // clang-format off
        return new SoundData(this->data.data() + start * this->channels * (this->bitDepth / 8), length, this->sampleRate, this->bitDepth, this->channels);
        // clang-format on
    }
} // namespace love
