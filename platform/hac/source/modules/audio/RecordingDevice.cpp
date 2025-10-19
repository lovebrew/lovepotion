#include "modules/audio/RecordingDevice.hpp"
#include "modules/audio/Audio.hpp"
#include "modules/sound/Sound.hpp"

#include "common/Result.hpp"

#include <malloc.h>

#define SOUND() (Module::getInstance<Sound>(Module::M_SOUND))

namespace love
{
    RecordingDevice::RecordingDevice(const char* name) :
        name(name),
        buffer {},
        releasedBuffers(0),
        releasedBuffer(nullptr)
    {}

    RecordingDevice::~RecordingDevice()
    {
        this->stop();
    }

    bool RecordingDevice::start(int samples, int sampleRate, int bitDepth, int channels)
    {
        if (bitDepth != 8 && bitDepth != 16)
            throw InvalidFormatException(channels, bitDepth);

        if (channels < 1 || channels > 2)
            throw InvalidFormatException(channels, bitDepth);

        if (samples <= 0)
            throw love::Exception("Invalid number of samples.");

        if (sampleRate <= 0)
            throw love::Exception("Invalid sample rate.");

        if (this->isRecording())
            this->stop();

        this->samples    = samples;
        this->sampleRate = sampleRate;
        this->bitDepth   = bitDepth;
        this->channels   = channels;

        const auto dataSize   = samples * (bitDepth / 8) * channels;
        const auto bufferSize = (dataSize + 0xFFF) & ~0xFFF;

        this->buffer.next   = nullptr;
        this->buffer.buffer = memalign(0x1000, bufferSize);

        if (!this->buffer.buffer)
            return false;

        std::memset(this->buffer.buffer, 0, bufferSize);

        this->buffer.buffer_size = bufferSize;
        this->buffer.data_size   = dataSize;
        this->buffer.data_offset = 0;

        this->releasedBuffer  = nullptr;
        this->releasedBuffers = 0;

        if (this->buffer.buffer == nullptr)
            throw love::Exception("Failed to allocate recording buffer.");

        if (!ResultCode(audinInitialize()))
            return false;

        if (!ResultCode(audinStartAudioIn()))
            return false;

        if (!ResultCode(audinAppendAudioInBuffer(&this->buffer)))
            return false;

        return true;
    }

    void RecordingDevice::stop()
    {
        if (!this->isRecording())
            return;

        audinWaitCaptureFinish(&releasedBuffer, &releasedBuffers, UINT64_MAX);
        audinStopAudioIn();
        audinExit();
    }

    SoundData* RecordingDevice::getData()
    {
        if (!this->isRecording())
            return nullptr;

        auto* data = SOUND()->newSoundData(this->samples, this->sampleRate, this->bitDepth, this->channels);

        if (!ResultCode(audinGetReleasedAudioInBuffer(&this->releasedBuffer, &this->releasedBuffers)))
            return nullptr;

        std::memcpy(data->getData(), this->releasedBuffer->buffer, this->releasedBuffer->data_size);

        return data;
    }

    const char* RecordingDevice::getName() const
    {
        return this->name.c_str();
    }

    int RecordingDevice::getSampleCount() const
    {
        if (!this->isRecording())
            return 0;

        AudioInBuffer* temp = nullptr;
        uint32_t tempCount  = 0;

        if (!ResultCode(audinGetReleasedAudioInBuffer(&temp, &tempCount)))
            return 0;

        const auto bytesPerFrame = (this->bitDepth / 8) * this->channels;
        const auto frames        = temp->data_offset / bytesPerFrame;

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
        AudioInState state;
        audinGetAudioInState(&state);

        return state == AudioInState_Started;
    }

    void RecordingDevice::setGain(uint8_t)
    {}

    uint8_t RecordingDevice::getGain() const
    {
        return 0;
    }
} // namespace love
