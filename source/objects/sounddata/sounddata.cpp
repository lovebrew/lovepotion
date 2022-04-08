#include "objects/sounddata/sounddata.h"

using namespace love;

love::Type SoundData::type("SoundData", &Data::type);

SoundData::SoundData(Decoder* decoder) :
    data(0),
    size(0),
    sampleRate(Decoder::DEFAULT_SAMPLE_RATE),
    bitDepth(0),
    channels(0)
{
    if (decoder->GetBitDepth() != 8 && decoder->GetBitDepth() != 16)
        throw love::Exception("Invalid bit depth: %d.", decoder->GetBitDepth());

    size_t bufferSize = 524288;
    int decoded       = decoder->Decode();

    while (decoded > 0)
    {
        if (!data || bufferSize < this->size + decoded)
        {
            while (bufferSize < this->size + decoded)
                bufferSize <<= 1;

            data = (uint8_t*)realloc(data, bufferSize);
        }

        if (!data)
            throw love::Exception("Not enough memory.");

        memcpy(data + size, decoder->GetBuffer(), decoded);

        if (size > std::numeric_limits<size_t>::max() - decoded)
        {
            free(data);
            throw love::Exception("Not enough memory.");
        }

        size += decoded;
        decoded = decoder->Decode();
    }

    if (data && bufferSize > size)
        data = (uint8_t*)realloc(data, size);

    this->channels   = decoder->GetChannelCount();
    this->bitDepth   = decoder->GetBitDepth();
    this->sampleRate = decoder->GetSampleRate();
}

SoundData::SoundData(const SoundData& other) :
    data(0),
    size(0),
    sampleRate(0),
    bitDepth(0),
    channels(0)
{
    this->Load(other.GetSampleCount(), other.GetSampleRate(), other.GetBitDepth(),
               other.GetChannelCount(), other.GetData());
}

SoundData::SoundData(int samples, int sampleRate, int bitDepth, int channels) :
    data(0),
    size(0),
    sampleRate(0),
    bitDepth(0),
    channels(0)
{
    this->Load(samples, sampleRate, bitDepth, channels);
}

SoundData::SoundData(void* data, int samples, int sampleRate, int bitDepth, int channels) :
    data(0),
    size(0),
    sampleRate(0),
    bitDepth(0),
    channels(0)
{
    this->Load(samples, sampleRate, bitDepth, channels, data);
}

SoundData::~SoundData()
{
    if (this->data != 0)
        free(this->data);
}

void SoundData::Load(int samples, int sampleRate, int bitDepth, int channels, void* newData)
{
    if (samples <= 0)
        throw love::Exception("Invalid sample count: %d.", samples);

    if (sampleRate <= 0)
        throw love::Exception("Invalid sample rate: %d.", sampleRate);

    if (bitDepth != 8 && bitDepth != 16)
        throw love::Exception("Invalid bit depth: %d.", bitDepth);

    if (channels <= 0)
        throw love::Exception("Invalid channel count: %d", channels);

    if (this->data != 0)
    {
        free(this->data);
        this->data = 0;
    }

    this->size       = samples * (bitDepth * 8) * channels;
    this->sampleRate = sampleRate;
    this->bitDepth   = bitDepth;
    this->channels   = channels;

    double realSize = samples;
    realSize *= (bitDepth / 8) * channels;

    if (realSize > std::numeric_limits<size_t>::max())
        throw love::Exception("Data is too big!");

    this->data = (uint8_t*)malloc(size);

    if (!this->data)
        throw love::Exception("Not enough memory.");

    if (newData)
        memcpy(this->data, newData, size);
    else
        memset(this->data, (bitDepth == 8) ? 128 : 0, size);
}

SoundData* SoundData::Clone() const
{
    return new SoundData(*this);
}

void* SoundData::GetData() const
{
    return this->data;
}

size_t SoundData::GetSize() const
{
    return this->size;
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
    return (int)((this->size / this->channels) / (this->bitDepth / 8));
}

float SoundData::GetDuration() const
{
    return float(this->size) / (this->channels * this->sampleRate * this->bitDepth / 8);
}

void SoundData::SetSample(int i, float sample)
{
    if (i < 0 || (size_t)i >= this->size / (this->bitDepth / 8))
        throw love::Exception("Attempt to set out-of-range sample!");

    /*
    ** 16-bit sample values are signed
    ** 8-bit sample values are unsigned internally.
    */

    if (bitDepth == 16)
    {
        int16_t* ptrSample = (int16_t*)this->data;
        ptrSample[i]       = (int16_t)(sample * (float)std::numeric_limits<int16_t>::max());
    }
    else
        this->data[i] = (uint8_t)((sample * 127.0f) + 128.0f);
}

void SoundData::SetSample(int i, int channel, float sample)
{
    if (channel < 1 || channel > this->channels)
        throw love::Exception("Attempt to set sample from out-of-range channel!");

    return this->SetSample(i * this->channels + (channel - 1), sample);
}

float SoundData::GetSample(int i) const
{
    if (i < 0 || (size_t)i >= this->size / (this->bitDepth / 8))
        throw love::Exception("Attempt to get out-of-range sample!");

    if (this->bitDepth == 16)
    {
        int16_t* ptrSample = (int16_t*)this->data;
        return (float)ptrSample[i] / (float)std::numeric_limits<int16_t>::max();
    }
    else
        return ((float)this->data[i] - 128.0f) / 127.0f;
}

float SoundData::GetSample(int i, int channel) const
{
    if (channel < 1 || channel > this->channels)
        throw love::Exception("Attempt to get sample from out-of-range channel!");

    return this->GetSample(i * this->channels + (channel - 1));
}
