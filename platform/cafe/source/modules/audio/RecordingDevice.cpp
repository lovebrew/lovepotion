#include "modules/audio/RecordingDevice.hpp"
#include "modules/sound/Sound.hpp"

#define SOUND() (Module::getInstance<Sound>(Module::M_SOUND))

namespace love
{
    RecordingDevice::RecordingDevice(const char* name) : name(name)
    {}

    RecordingDevice::~RecordingDevice()
    {}

    const char* RecordingDevice::getName() const
    {
        return this->name.c_str();
    }

    bool RecordingDevice::start(int samples, int sampleRate, int bitDepth, int channels)
    {
        return false;
    }

    void RecordingDevice::stop()
    {
        if (!this->isRecording())
            return;
    }

    SoundData* RecordingDevice::getData()
    {
        if (!this->isRecording())
            return nullptr;

        auto* data = SOUND()->newSoundData(samples, this->sampleRate, this->bitDepth, this->channels);

        if (!data)
            return nullptr;

        return data;
    }

    void RecordingDevice::setGain(uint8_t gain)
    {}

    uint8_t RecordingDevice::getGain() const
    {
        return 0;
    }

    int RecordingDevice::getSampleCount() const
    {
        if (!this->isRecording())
            return 0;

        return 0;
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
        return sampling;
    }
} // namespace love
