#include "common/Exception.hpp"
#include "common/config.hpp"

#include "modules/audio/Source.hpp"

#include "modules/audio/Pool.hpp"

#include <algorithm>

namespace love
{
    Type Source::type("Source", &Object::type);

    class InvalidFormatException : public love::Exception
    {
      public:
        InvalidFormatException(int channels, int bitdepth) :
            Exception(E_INVALID_AUDIO_FORMAT, channels, bitdepth)
        {}
    };

    class QueueFormatMismatchException : public love::Exception
    {
      public:
        QueueFormatMismatchException() : Exception(E_QUEUE_FORMAT_MISMATCH)
        {}
    };

    class QueueTypeMismatchException : public love::Exception
    {
      public:
        QueueTypeMismatchException() : Exception(E_QUEUE_TYPE_MISMATCH)
        {}
    };

    class QueueMalformedLengthException : public love::Exception
    {
      public:
        QueueMalformedLengthException(int bytes) : Exception(E_QUEUE_LENGTH_MALFORMED, bytes)
        {}
    };

    class QueueLoopingException : public love::Exception
    {
      public:
        QueueLoopingException() : Exception(E_QUEUE_CANNOT_BE_LOOPED)
        {}
    };

    Source::Source(Pool* pool, SoundData* soundData) :
        sourceType(TYPE_STATIC),
        pool(pool),
        sampleRate(soundData->getSampleRate()),
        channels(soundData->getChannelCount()),
        bitDepth(soundData->getBitDepth())
    {
        if (DigitalSound::getFormat(soundData->getChannelCount(), soundData->getBitDepth()) < 0)
            throw InvalidFormatException(soundData->getChannelCount(), soundData->getBitDepth());

        this->staticBuffer.set(new StaticDataBuffer(soundData), Acquire::NO_RETAIN);
    }

    Source::Source(Pool* pool, Decoder* decoder) :
        sourceType(TYPE_STREAM),
        pool(pool),
        sampleRate(decoder->getSampleRate()),
        channels(decoder->getChannelCount()),
        bitDepth(decoder->getBitDepth()),
        buffers(DEFAULT_BUFFERS),
        decoder(decoder)
    {
        if (DigitalSound::getFormat(decoder->getChannelCount(), decoder->getBitDepth()) < 0)
            throw InvalidFormatException(decoder->getChannelCount(), decoder->getBitDepth());

        // clang-format off
        for (int index = 0; index < this->buffers; index++)
            this->streamBuffers.push_back(DigitalSound::getInstance().createBuffer(decoder->getSize(), this->channels));
        // clang-format on
    }

    Source::Source(Pool* pool, int sampleRate, int bitDepth, int channels, int buffers) :
        sourceType(TYPE_QUEUE),
        pool(pool),
        sampleRate(sampleRate),
        channels(channels),
        bitDepth(bitDepth),
        buffers(buffers)
    {
        if (DigitalSound::getFormat(channels, bitDepth) < 0)
            throw InvalidFormatException(channels, bitDepth);

        this->buffers = std::clamp(buffers, 1, MAX_BUFFERS);

        // clang-format off
        for (int index = 0; index < this->buffers; index++)
            this->streamBuffers.push_back(DigitalSound::getInstance().createBuffer(decoder->getSize(), this->channels));
        // clang-format on
    }

    Source::Source(const Source& other) :
        Object(other),
        sourceType(other.sourceType),
        pool(other.pool),
        valid(false),
        staticBuffer(other.staticBuffer),
        pitch(other.pitch),
        volume(other.volume),
        looping(other.looping),
        minVolume(other.minVolume),
        maxVolume(other.maxVolume),
        offsetSamples(0),
        sampleRate(other.sampleRate),
        channels(other.channels),
        bitDepth(other.bitDepth),
        buffers(other.buffers),
        decoder(nullptr)
    {
        if (this->sourceType == TYPE_STREAM)
        {
            if (other.decoder.get())
                this->decoder.set(other.decoder->clone(), Acquire::NO_RETAIN);
        }

        // clang-format off
        if (this->sourceType != TYPE_STATIC)
        {
            for (int index = 0; index < this->buffers; index++)
                this->streamBuffers.push_back(DigitalSound::getInstance().createBuffer(decoder->getSize(), this->channels));
        }
        // clang-format on
    }

    Source::~Source()
    {
        this->stop();

        if (this->sourceType != TYPE_STATIC)
        {
            for (size_t index = 0; index < this->streamBuffers.size(); index++)
                DigitalSound::getInstance().freeBuffer(this->streamBuffers[index]);
        }
    }

    bool Source::update()
    {
        if (!this->valid)
            return false;

        switch (this->sourceType)
        {
            case TYPE_STATIC:
                return !this->isFinished();
            case TYPE_STREAM:
            {
                if (this->isFinished())
                    return false;

                for (auto& buffer : this->streamBuffers)
                {
                    if (!DigitalSound::getInstance().isBufferDone(buffer))
                        continue;

                    if (this->streamAtomic(buffer, this->decoder.get()) == 0)
                        break;

                    DigitalSound::getInstance().channelAddBuffer(this->channel, &buffer);
                    this->offsetSamples += DigitalSound::getInstance().getSampleCount(buffer);
                }

                return true;
            }
            case TYPE_QUEUE:
                return !this->isFinished();
            default:
                break;
        }

        return false;
    }

    Source* Source::clone()
    {
        return new Source(*this);
    }

    bool Source::play()
    {
        uint8_t wasPlaying;

        {
            auto lock = this->pool->lock();

            if (!this->pool->assignSource(this, this->channel, wasPlaying))
                return this->valid = false;
        }

        if (!wasPlaying)
        {
            if (!(this->valid = this->playAtomic()))
                return false;

            this->resumeAtomic();

            {
                auto lock = this->pool->lock();
                this->pool->addSource(this, this->channel);
            }

            return this->valid;
        }

        this->resumeAtomic();
        return this->valid = true;
    }

    // clang-format off
    void Source::reset()
    {
        if (!DigitalSound::getInstance().channelReset(this->channel, this->channels, this->bitDepth, this->sampleRate))
            std::printf("Failed to reset channel %zu\n", this->channel);
    }
    // clang-format on

    void Source::stop()
    {
        if (!this->valid)
            return;

        auto lock = this->pool->lock();
        this->pool->releaseSource(this);
    }

    bool Source::play(const std::vector<Source*>& sources)
    {
        // if (sources.size() == 0)
        //     return true;

        // auto* pool = ((Source*)sources[0])->pool;
        // auto lock  = pool->lock();

        // std::vector<uint8_t> wasPlaying(sources.size());
        // std::vector<size_t> channels(sources.size());

        // for (size_t index = 0; index < sources.size(); index++)
        // {
        //     auto* source = (Source*)sources[index];

        //     if (!pool->assignSource(source, channels[index], wasPlaying[index]))
        //     {
        //         for (size_t j = 0; j < index; j++)
        //         {
        //             if (!wasPlaying[j])
        //                 pool->releaseSource(sources[j], false);

        //             return false;
        //         }
        //     }
        // }

        // std::vector<size_t> channelsToPlay;
        // channelsToPlay.reserve(sources.size());

        // for (size_t index = 0; index < sources.size(); index++)
        // {
        //     if (wasPlaying[index] && sources[index]->isPlaying())
        //         continue;

        //     if (!wasPlaying[index])
        //     {
        //         auto* source    = (Source*)sources[index];
        //         source->channel = channels[index];
        //         source->prepareAtomic();
        //     }

        //     channelsToPlay.push_back(channels[index]);
        // }

        // bool success = false;
        // for (auto& channel : channelsToPlay)
        // {
        //     if (DigitalSound::getInstance().channelAddBuffer(channel, sources[channel]))
        // }
        return false;
    }

    void Source::stop(const std::vector<Source*>& sources)
    {
        if (sources.size() == 0)
            return;

        auto pool = ((Source*)sources[0])->pool;
        auto lock = pool->lock();

        for (auto& _source : sources)
        {
            auto* source = (Source*)_source;

            if (source->valid)
                source->teardownAtomic();

            pool->releaseSource(source, false);
        }
    }

    void Source::pause(const std::vector<Source*>& sources)
    {
        if (sources.size() == 0)
            return;

        auto lock = ((Source*)sources[0])->pool->lock();

        std::vector<size_t> channels;
        channels.reserve(sources.size());

        for (auto& _source : sources)
        {
            auto* source = (Source*)_source;

            if (source->valid)
                channels.push_back(source->channel);
        }

        for (auto& channel : channels)
            DigitalSound::getInstance().channelPause(channel, true);
    }

    std::vector<Source*> Source::pause(Pool* pool)
    {
        auto lock    = pool->lock();
        auto sources = pool->getPlayingSources();

        // clang-format off
        auto end = std::remove_if(sources.begin(), sources.end(), [](Source* source) {
            return !source->isPlaying();
        });
        // clang-format on

        sources.erase(end, sources.end());
        Source::pause(sources);

        return sources;
    }

    void Source::pause()
    {
        auto lock = this->pool->lock();

        if (this->pool->isPlaying(this))
            this->pauseAtomic();
    }

    void Source::stop(Pool* pool)
    {
        auto lock = pool->lock();
        Source::stop(pool->getPlayingSources());
    }

    bool Source::isPlaying() const
    {
        if (!this->valid)
            return false;

        return DigitalSound::getInstance().isChannelPaused(this->channel) == false;
    }

    bool Source::isFinished() const
    {
        if (!this->valid)
            return false;

        if (this->sourceType == TYPE_STREAM && (this->isLooping() || !this->decoder->isFinished()))
            return false;

        return DigitalSound::getInstance().isChannelPlaying(this->channel) == false;
    }

    void Source::setVolume(float volume)
    {
        if (this->valid)
            DigitalSound::getInstance().channelSetVolume(this->channel, this->volume);

        this->volume = volume;
    }

    float Source::getVolume() const
    {
        if (this->valid)
            return DigitalSound::getInstance().channelGetVolume(this->channel);

        return this->volume;
    }

    void Source::seek(double offset, Unit unit)
    {
        auto lock = this->pool->lock();

        int offsetSamples    = 0;
        double offsetSeconds = 0.0;

        switch (unit)
        {
            case UNIT_SAMPLES:
            {
                offsetSamples = (int)offset;
                offsetSeconds = offset / ((double)this->sampleRate / this->channels);
                break;
            }
            case UNIT_SECONDS:
            default:
            {
                offsetSamples = (int)(offset * this->sampleRate * this->channels);
                offsetSeconds = offset;
                break;
            }
        }

        bool wasPlaying = this->isPlaying();

        switch (this->sourceType)
        {
            case TYPE_STATIC:
            {
                if (this->valid)
                    this->stop();

                this->offsetSamples = offsetSamples;

                if (wasPlaying)
                    this->play();

                break;
            }
            case TYPE_STREAM:
            {
                if (this->valid)
                    this->stop();

                this->decoder->seek(offsetSeconds);

                if (wasPlaying)
                    this->play();

                break;
            }
            case TYPE_QUEUE:
                break;
            default:
                break;
        }

        if (wasPlaying && (this->sourceType == TYPE_STREAM && !this->isPlaying()))
        {
            this->stop();

            if (this->isLooping())
                this->play();

            return;
        }

        this->offsetSamples = offsetSamples;
    }

    double Source::tell(Unit unit)
    {
        auto lock = this->pool->lock();

        int offset = 0;

        if (this->valid)
        {
            if (this->sourceType == TYPE_STATIC)
                offset = DigitalSound::getInstance().channelGetSampleOffset(this->channel);
            else
                offset = this->offsetSamples;
        }

        return (unit == UNIT_SAMPLES) ? offset : (offset / this->sampleRate);
    }

    double Source::getDuration(Unit unit)
    {
        auto lock = this->pool->lock();

        switch (this->sourceType)
        {
            case TYPE_STATIC:
            {
                const auto size = this->staticBuffer->getSize();
                const auto nsamples =
                    ((size / this->channels) / (this->bitDepth / 8)) - (this->offsetSamples / this->channels);

                return (unit == UNIT_SAMPLES) ? nsamples : (nsamples / this->sampleRate);
            }
            case TYPE_STREAM:
            {
                double seconds = this->decoder->getDuration();
                return (unit == UNIT_SAMPLES) ? (seconds * this->sampleRate) : seconds;
            }
            case TYPE_QUEUE:
                return 0.0;
            default:
                return 0.0;
        }

        return 0.0;
    }

    int Source::getFreeBufferCount() const
    {
        switch (this->sourceType)
        {
            case TYPE_STATIC:
                return 0;
            case TYPE_STREAM:
            case TYPE_QUEUE:
            {
                int count = 0;

                for (const auto& buffer : this->streamBuffers)
                {
                    if (DigitalSound::getInstance().isBufferDone(buffer))
                        count++;
                }

                return count;
            }
            case TYPE_MAX_ENUM:
                return 0;
        }

        return 0;
    }

    void Source::setLooping(bool looping)
    {
        if (this->sourceType == TYPE_QUEUE)
            throw QueueLoopingException();

        if (this->valid && this->sourceType == TYPE_STATIC)
            this->staticBuffer->setLooping(this->looping);

        this->looping = looping;
    }

    bool Source::isLooping() const
    {
        return this->looping;
    }

    // clang-format off
    void Source::prepareAtomic()
    {
        this->reset();

        switch (this->sourceType)
        {
            case TYPE_STATIC:
            {
                auto& buffer = this->staticBuffer->clone(this->offsetSamples, this->channels);
                DigitalSound::getInstance().channelAddBuffer(this->channel, &buffer);

                break;
            }
            case TYPE_STREAM:
            {
                for (auto& buffer : this->streamBuffers)
                {
                    if (this->streamAtomic(buffer, this->decoder.get()) == 0)
                        break;

                    if (this->decoder->isFinished())
                        break;
                }
                break;
            }
            case TYPE_QUEUE:
                break;
            default:
                break;
        }
    }
    // clang-format on

    void Source::teardownAtomic()
    {
        DigitalSound::getInstance().channelStop(this->channel);

        switch (this->sourceType)
        {
            case TYPE_STATIC:
                break;
            case TYPE_STREAM:
            {
                this->decoder->rewind();
                DigitalSound::getInstance().channelStop(this->channel);

                break;
            }
            case TYPE_QUEUE:
                DigitalSound::getInstance().channelStop(this->channel);
                break;
            default:
                break;
        }

        this->valid         = false;
        this->offsetSamples = 0;
    }

    int Source::streamAtomic(AudioBuffer& buffer, Decoder* decoder)
    {
        int decoded = std::max(decoder->decode(), 0);

        if (decoded > 0)
        {
            // DigitalSound::getInstance().setLooping(buffer, this->looping);

            const int samples = int((decoded / this->channels) / (this->bitDepth / 8));
            DigitalSound::getInstance().prepare(buffer, decoder->getBuffer(), decoded, samples);
        }

        if (decoder->isFinished() && this->isLooping())
            decoder->rewind();

        return decoded;
    }

    bool Source::playAtomic()
    {
        this->prepareAtomic();

        if (this->sourceType != TYPE_STATIC)
        {
            for (auto& buffer : this->streamBuffers)
                DigitalSound::getInstance().channelAddBuffer(this->channel, &buffer);
        }

        bool success = false;
        if (this->sourceType == TYPE_STREAM)
        {
            this->valid = true;

            if (!this->isPlaying())
                success = false;
        }

        if (!success)
        {
            this->valid = true;
            this->stop();
        }

        if (this->sourceType != TYPE_STREAM)
            this->offsetSamples = 0;

        return true;
    }

    void Source::stopAtomic()
    {
        if (!this->valid)
            return;

        this->teardownAtomic();
    }

    void Source::pauseAtomic()
    {
        if (!this->valid)
            return;

        DigitalSound::getInstance().channelPause(this->channel, true);
    }

    void Source::resumeAtomic()
    {
        if (!this->valid)
            return;

        DigitalSound::getInstance().channelPause(this->channel, false);
    }
} // namespace love
