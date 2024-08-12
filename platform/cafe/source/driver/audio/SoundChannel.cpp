#include "driver/audio/SoundChannel.hpp"
#include "driver/audio/AudioBuffer.hpp"

#include "driver/audio/DigitalSound.hpp"
#include "driver/audio/DigitalSoundMix.hpp"

namespace love
{
    SoundChannel::SoundChannel() :
        buffers {},
        state(STATE_DONE),
        ratio(1.0f),
        volume(1.0f),
        format(AX_VOICE_FORMAT_LPCM16),
        channels(2),
        samplePosition(0)
    {}

    void SoundChannel::reset()
    {
        while (!this->buffers.empty())
            this->buffers.pop();

        this->state          = STATE_DONE;
        this->channels       = 2;
        this->format         = AX_VOICE_FORMAT_LPCM16;
        this->ratio          = 1.0f;
        this->volume         = 1.0f;
        this->samplePosition = 0;
    }

    bool SoundChannel::isPlaying() const
    {
        return this->state == STATE_PLAYING;
    }

    size_t SoundChannel::getSamplePosition() const
    {
        return this->samplePosition;
    }

    void SoundChannel::setFormat(AXVoiceFormat format)
    {
        this->format = format;
    }

    AXVoiceFormat SoundChannel::getFormat() const
    {
        return this->format;
    }

    bool SoundChannel::isPaused() const
    {
        return this->state == STATE_PAUSED;
    }

    void SoundChannel::setPaused(bool paused)
    {
        if (this->buffers.empty())
            return;

        auto state = SoundChannel::getState(paused);
        this->buffers.front()->setState(state);

        this->state = paused ? STATE_PAUSED : STATE_PLAYING;
    }

    void SoundChannel::setSampleRate(float sampleRate)
    {
        this->ratio = sampleRate / (float)AXGetInputSamplesPerSec();
    }

    float SoundChannel::getSampleRate() const
    {
        return this->ratio * (float)AXGetInputSamplesPerSec();
    }

    void SoundChannel::setVolume(float volume)
    {
        this->volume = volume;
    }

    float SoundChannel::getVolume() const
    {
        return this->volume;
    }

    void SoundChannel::setInterpType(int channels)
    {
        this->channels = channels;
    }

    int SoundChannel::getInterpType() const
    {
        return this->channels;
    }

    bool SoundChannel::addWaveBuffer(AudioBuffer* buffer)
    {
        if (!buffer)
            return false;

        if (!buffer->isInitialized())
            buffer->initialize(this->channels);

        if (!buffer->prepare())
            return false;

        if (!buffer->setFormat(this->format))
            return false;

        buffer->setVolume(this->volume);

        if (!buffer->setSampleRateConversionRatio(this->ratio))
            return false;

        buffer->setDeviceMix(this->channels);

        this->buffers.push(buffer);

        return true;
    }

    void SoundChannel::update()
    {
        if (this->buffers.empty())
        {
            this->state = STATE_DONE;
            return;
        }

        auto* buffer = this->buffers.front();

        if (!buffer->isPlaying())
            this->play();

        if (buffer->isFinished())
        {
            buffer->setState(AX_VOICE_STATE_STOPPED);
            this->buffers.pop();
        }
    }

    void SoundChannel::play()
    {
        if (this->buffers.empty())
            return;

        auto* buffer = this->buffers.front();

        buffer->setState(AX_VOICE_STATE_PLAYING);
        this->state = STATE_PLAYING;
    }

    void SoundChannel::stop()
    {
        while (!this->buffers.empty())
        {
            this->buffers.front()->setState(AX_VOICE_STATE_STOPPED);
            this->buffers.pop();
        }

        this->state = STATE_DONE;
    }

    AXVoiceState SoundChannel::getState(bool stopped)
    {
        return stopped ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;
    }
} // namespace love
