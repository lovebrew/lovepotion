#include "modules/audio/RecordingDevice.hpp"
#include "modules/audio/dsp/Audio.hpp"
#include "modules/sound/Sound.hpp"

#include "common/Exception.hpp"
#include "common/Result.hpp"

#include <malloc.h>

#define SOUND() (Module::getInstance<Sound>(Module::M_SOUND))

namespace love
{
    MICU_Encoding RecordingDevice::getFormat(int bitDepth, int channels)
    {
        if (bitDepth != 8 && bitDepth != 16)
            return MICU_Encoding(-1);

        if (channels < 1 || channels > 2)
            return MICU_Encoding(-2);

        return (bitDepth == 8) ? MICU_ENCODING_PCM8_SIGNED : MICU_ENCODING_PCM16_SIGNED;
    }

    uint8_t* RecordingDevice::BUFFER   = nullptr;
    uint32_t RecordingDevice::POSITION = 0;

    RecordingDevice::RecordingDevice(const char* name) : name(name)
    {
        RecordingDevice::BUFFER = (uint8_t*)std::aligned_alloc(0x1000, RecordingDevice::BUFFER_SIZE);

        if (RecordingDevice::BUFFER == nullptr)
            throw love::Exception("Failed to allocate recording buffer.");

        if (!ResultCode(micInit(RecordingDevice::BUFFER, RecordingDevice::BUFFER_SIZE)))
            throw love::Exception("Failed to initialize microphone.");
    }

    RecordingDevice::~RecordingDevice()
    {
        this->stop();

        micExit();

        if (RecordingDevice::BUFFER != nullptr)
            std::free(RecordingDevice::BUFFER);

        RecordingDevice::BUFFER = nullptr;
    }

    const char* RecordingDevice::getName() const
    {
        return this->name.c_str();
    }

    static MICU_Encoding getEncoding(AudioBase::AudioFormat format)
    {
        switch (format)
        {
            default:
            case AudioBase::FORMAT_MONO8:
            case AudioBase::FORMAT_STEREO8:
                return MICU_ENCODING_PCM8_SIGNED;
            case AudioBase::FORMAT_MONO16:
            case AudioBase::FORMAT_STEREO16:
                return MICU_ENCODING_PCM16_SIGNED;
        }
    }

    bool RecordingDevice::start(int samples, int sampleRate, int bitDepth, int channels)
    {
        const auto format = audio::dsp::Audio::getFormat(bitDepth, channels);

        if (format == AudioBase::FORMAT_NONE)
            throw InvalidFormatException(channels, bitDepth);

        MICU_Encoding encoding = getEncoding(format);

        if (samples <= 0)
            throw love::Exception("Invalid number of samples.");

        if (sampleRate <= 0)
            throw love::Exception("Invalid sample rate.");

        if (this->isRecording())
            this->stop();

        MICU_SampleRate samplerate;
        if (!RecordingDevice::getConstant((SampleRate)sampleRate, samplerate))
            throw love::Exception("Unsupported sample rate: %d Hz", sampleRate);

        const auto dataSize = micGetSampleDataSize();

        if (!ResultCode(MICU_StartSampling(encoding, samplerate, 0, dataSize, false)))
            return false;

        this->samples    = samples;
        this->sampleRate = sampleRate;
        this->bitDepth   = bitDepth;
        this->channels   = channels;

        return true;
    }

    void RecordingDevice::stop()
    {
        if (!this->isRecording())
            return;

        MICU_StopSampling();
    }

    SoundData* RecordingDevice::getData()
    {
        if (!this->isRecording())
            return nullptr;

        const auto bytesPerFrame = (this->bitDepth / 8) * this->channels;

        const auto offset = micGetLastSampleOffset();
        size_t frames     = 0;

        if (offset >= RecordingDevice::POSITION)
            frames = (offset - RecordingDevice::POSITION) / bytesPerFrame;
        else
            frames = (RecordingDevice::BUFFER_SIZE - RecordingDevice::POSITION + offset) / bytesPerFrame;

        if (frames == 0)
            return nullptr;

        const auto samples = frames * this->channels;

        auto* data = SOUND()->newSoundData(samples, this->sampleRate, this->bitDepth, this->channels);

        if (!data)
            return nullptr;

        const auto size = frames * bytesPerFrame;
        auto* dst       = (uint8_t*)data->getData();

        if (RecordingDevice::POSITION + size <= RecordingDevice::BUFFER_SIZE)
            std::memcpy(dst, &RecordingDevice::BUFFER[RecordingDevice::POSITION], size);
        else
        {
            const auto firstPartSize  = RecordingDevice::BUFFER_SIZE - RecordingDevice::POSITION;
            const auto secondPartSize = size - firstPartSize;

            std::memcpy(dst, &RecordingDevice::BUFFER[RecordingDevice::POSITION], firstPartSize);
            std::memcpy(dst + firstPartSize, &RecordingDevice::BUFFER[0], secondPartSize);
        }

        // if we want stereo, copy the mono data to the second channel
        if (this->channels == 2)
        {
            for (int index = 0; index < data->getSampleCount(); index++)
            {
                float sample = data->getSample(index, 1);
                data->setSample(index, 2, sample);
            }
        }

        RecordingDevice::POSITION = (RecordingDevice::POSITION + size) % RecordingDevice::BUFFER_SIZE;

        return data;
    }

    void RecordingDevice::setGain(uint8_t gain)
    {
        MICU_SetGain(gain);
    }

    uint8_t RecordingDevice::getGain() const
    {
        uint8_t gain = 0;
        MICU_GetGain(&gain);

        return gain;
    }

    int RecordingDevice::getSampleCount() const
    {
        if (!this->isRecording())
            return 0;

        const auto bytesPerFrame = (this->bitDepth / 8) * this->channels;
        const auto frames        = micGetLastSampleOffset() / bytesPerFrame;

        return frames * this->channels;
    }

    int RecordingDevice::getMaxSamples() const
    {
        return this->samples;
    }

    int RecordingDevice::getSampleRate() const
    {
        return this->sampleRate;
    }

    int RecordingDevice::getBitDepth() const
    {
        return this->bitDepth;
    }

    int RecordingDevice::getChannelCount() const
    {
        return this->channels;
    }

    bool RecordingDevice::isRecording() const
    {
        bool sampling = false;
        MICU_IsSampling(&sampling);

        return sampling;
    }
} // namespace love
