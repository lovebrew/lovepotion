#include <common/exception.hpp>
#include <objects/file/file.hpp>

#include <algorithm>
#include <limits>

#include <objects/data/sounddata/sounddata.hpp>

using namespace love;

Type SoundData::type("SoundData", &Data::type);

SoundData::SoundData(Decoder* decoder) :
    sampleRate(Decoder::DEFAULT_SAMPLE_RATE),
    bitDepth(0),
    channels(0)
{
    if (decoder->GetBitDepth() != 8 && decoder->GetBitDepth() != 16)
        throw love::Exception("Invalid bit depth: %d", decoder->GetBitDepth());

    int decoded = decoder->Decode();

    while (decoded > 0)
    {
        try
        {
            this->buffer.resize(this->buffer.size() + decoded);
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception("Not enough memory.");
        }

        std::copy_n((uint8_t*)decoder->GetBuffer(), decoded,
                    this->buffer.data() + (this->buffer.size() - decoded));

        decoded = decoder->Decode();
    }

    /* shrink buffer if needed */
    this->buffer.shrink_to_fit();

    this->channels   = decoder->GetChannelCount();
    this->bitDepth   = decoder->GetBitDepth();
    this->sampleRate = decoder->GetSampleRate();
}

SoundData::SoundData(int samples, int sampleRate, int bitDepth, int channels) :
    sampleRate(0),
    bitDepth(0),
    channels(0)
{
    this->Load(samples, sampleRate, bitDepth, channels);
}

SoundData::SoundData(void* data, int samples, int sampleRate, int bitDepth, int channels) :
    sampleRate(0),
    bitDepth(0),
    channels(0)
{
    this->Load(samples, sampleRate, bitDepth, channels, data);
}

SoundData::SoundData(const SoundData& other) : sampleRate(0), bitDepth(0), channels(0)
{
    this->Load(other.GetSampleCount(), other.GetSampleRate(), other.GetBitDepth(),
               other.GetChannelCount(), other.GetData());
}

SoundData* SoundData::Clone() const
{
    return new SoundData(*this);
}

void SoundData::Load(int samples, int sampleRate, int bitDepth, int channels, void* newData)
{
    if (samples <= 0)
        throw love::Exception("Invalid sample count: %d", samples);

    if (sampleRate <= 0)
        throw love::Exception("Invalid sample rate: %d", sampleRate);

    if (bitDepth != 8 && bitDepth != 16)
        throw love::Exception("Invalid bit depth: %d", bitDepth);

    if (channels <= 0)
        throw love::Exception("Invalid channel count: %d", channels);

    this->buffer.clear();

    this->size       = samples * (bitDepth / 8) * channels;
    this->sampleRate = sampleRate;
    this->bitDepth   = bitDepth;
    this->channels   = channels;

    try
    {
        this->buffer.resize(size);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Not enough memory.");
    }

    if (newData)
        std::copy_n((uint8_t*)newData, size, this->buffer.data());
    else
        std::fill_n(this->buffer.data(), size, bitDepth == 8 ? 128 : 0);
}

void* SoundData::GetData() const
{
    return (void*)this->buffer.data();
}

size_t SoundData::GetSize() const
{
    return this->buffer.size();
}

int SoundData::GetChannelCount() const
{
    return this->channels;
}

int SoundData::GetBitDepth() const
{
    return this->bitDepth;
}

int SoundData::GetSampleRate() const
{
    return this->sampleRate;
}

int SoundData::GetSampleCount() const
{
    return (int)(((this->buffer.size() / this->channels)) / (this->bitDepth / 8));
}

float SoundData::GetDuration() const
{
    return (float)this->buffer.size() / (this->channels * this->sampleRate * this->bitDepth / 8);
}

void SoundData::SetSample(int index, float sample)
{
    if (index < 0 || (size_t)index >= this->GetSize() / (this->bitDepth / 8))
        throw love::Exception("Attempt to set out-of-range sample!");

    if (bitDepth == 16)
    {
        auto data   = (int16_t*)this->buffer.data();
        data[index] = (int16_t)(sample * (float)std::numeric_limits<int16_t>::max());
    }
    else
        this->buffer[index] = (uint8_t)((sample * 127.0f) + 128.0f);
}

void SoundData::SetSample(int index, int channel, float sample)
{
    if (channel < 1 || channel > this->channels)
        throw love::Exception("Attempt to set sample from out-of-range channel!");

    return this->SetSample(index * this->channels + (channel - 1), sample);
}

float SoundData::GetSample(int index) const
{
    if (index < 0 || (size_t)index >= this->GetSize() / (this->bitDepth / 8))
        throw love::Exception("Attempt to get out-of-range sample!");

    if (this->GetBitDepth() == 16)
    {
        auto data = (int16_t*)this->buffer.data();
        return (float)data[index] / (float)std::numeric_limits<int16_t>::max();
    }
    else
        return ((float)this->buffer[index] - 128.0f) / 127.0f;
}

float SoundData::GetSample(int index, int channel) const
{
    if (channel == 1 || channel > this->channels)
        throw love::Exception("Attempt to get sample from out-of-range channel!");

    return this->GetSample(index * this->channels + (channel - 1));
}
