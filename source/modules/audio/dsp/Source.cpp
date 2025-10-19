#include "common/Exception.hpp"
#include "common/config.hpp"

#include "modules/audio/dsp/Audio.hpp"
#include "modules/audio/dsp/Pool.hpp"
#include "modules/audio/dsp/Source.hpp"

#include <algorithm>

namespace love
{
    namespace audio
    {
        namespace dsp
        {
            StaticDataBuffer::StaticDataBuffer(const SoundData* data) :
                size(data->getSize()),
                nsamples(data->getSampleCount())
            {
                this->buffer = audio::Buffer(data->getSize(), data->getChannelCount());
                this->buffer.prepare(data->getData(), data->getSize(), data->getSampleCount());
            }

            StaticDataBuffer::~StaticDataBuffer()
            {
                this->buffer.destroy();
            }

            audio::Buffer& StaticDataBuffer::getView(const size_t offset, int channels)
            {
                const auto samples = this->nsamples - (offset / channels);
                this->view.prepare(this->buffer.getData() + offset, this->size, samples, false);
                this->view.setLooping(this->buffer.isLooping());

                return this->view;
            }

            void StaticDataBuffer::setLooping(bool looping)
            {
                this->buffer.setLooping(looping);
            }

            size_t StaticDataBuffer::getSize() const
            {
                return this->size;
            }

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

            Source::Source(Pool* pool, SoundData* data) :
                SourceBase(TYPE_STATIC),
                pool(pool),
                samplerate(data->getSampleRate()),
                channels(data->getChannelCount()),
                bitdepth(data->getBitDepth())
            {
                if (Audio::getFormat(data->getBitDepth(), data->getChannelCount()) == Audio::FORMAT_NONE)
                    throw InvalidFormatException(data->getChannelCount(), data->getBitDepth());

                this->staticBuffer.set(new StaticDataBuffer(data), Acquire::NO_RETAIN);
            }

            Source::Source(Pool* pool, Decoder* decoder) :
                SourceBase(TYPE_STREAM),
                pool(pool),
                samplerate(decoder->getSampleRate()),
                channels(decoder->getChannelCount()),
                bitdepth(decoder->getBitDepth()),
                decoder(decoder),
                buffers(DEFAULT_BUFFERS)
            {
                if (Audio::getFormat(decoder->getBitDepth(), decoder->getChannelCount()) ==
                    Audio::FORMAT_NONE)
                    throw InvalidFormatException(decoder->getChannelCount(), decoder->getBitDepth());

                for (int index = 0; index < this->buffers; index++)
                    this->streamBuffers.push_back(audio::Buffer(decoder->getSize(), this->channels));
            }

            Source::Source(Pool* pool, int sampleRate, int bitDepth, int channels, int buffers) :
                SourceBase(TYPE_QUEUE),
                pool(pool),
                samplerate(sampleRate),
                channels(channels),
                bitdepth(bitDepth),
                buffers(buffers)
            {
                if (Audio::getFormat(bitDepth, channels) == Audio::FORMAT_NONE)
                    throw InvalidFormatException(channels, bitDepth);

                this->buffers = std::clamp(buffers, 1, MAX_BUFFERS);

                for (int index = 0; index < this->buffers; index++)
                    this->streamBuffers.push_back(audio::Buffer(decoder->getSize(), this->channels));
            }

            Source::Source(const Source& other) :
                SourceBase(other.sourceType),
                pool(other.pool),
                valid(false),
                staticBuffer(other.staticBuffer),
                pitch(other.pitch),
                volume(other.volume),
                looping(other.looping),
                minVolume(other.minVolume),
                maxVolume(other.maxVolume),
                offsetSamples(0),
                samplerate(other.samplerate),
                channels(other.channels),
                bitdepth(other.bitdepth),
                decoder(nullptr),
                buffers(other.buffers)
            {
                if (this->sourceType == TYPE_STREAM)
                {
                    if (other.decoder.get())
                        this->decoder.set(other.decoder->clone(), Acquire::NO_RETAIN);
                }

                if (this->sourceType != TYPE_STATIC)
                {
                    for (int index = 0; index < this->buffers; index++)
                        this->streamBuffers.push_back(audio::Buffer(decoder->getSize(), this->channels));
                }
            }

            Source::~Source()
            {
                this->stop();

                if (this->sourceType != TYPE_STATIC)
                {
                    while (!this->streamBuffers.empty())
                    {
                        this->streamBuffers.front().destroy();
                        this->streamBuffers.pop_back();
                    }
                    while (!this->unusedBuffers.empty())
                    {
                        this->unusedBuffers.top().destroy();
                        this->unusedBuffers.pop();
                    }
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
                            if (!buffer.isFinished())
                                continue;

                            if (this->streamAtomic(buffer, this->decoder.get()) == 0)
                                break;

                            audio::Channel::addBuffer(this->channel, buffer);
                            this->offsetSamples += buffer.getSampleCount();
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

            void Source::setPitch(float pitch)
            {
                this->pitch = pitch;
            }

            float Source::getPitch() const
            {
                return this->pitch;
            }

            SourceBase* Source::clone()
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

            void Source::reset()
            {
                const auto rate = this->samplerate * this->pitch;
                audio::Channel::reset(this->channel, this->channels, this->bitdepth, rate, this->volume);
            }

            void Source::stop()
            {
                if (!this->valid)
                    return;

                auto lock = this->pool->lock();
                this->pool->releaseSource(this);
            }

            bool Source::play(const std::vector<SourceBase*>& sources)
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

            void Source::stop(const std::vector<SourceBase*>& sources)
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

            void Source::pause(const std::vector<SourceBase*>& sources)
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
                    audio::Channel::pause(channel, true);
            }

            std::vector<SourceBase*> Source::pause(Pool* pool)
            {
                auto lock    = pool->lock();
                auto sources = pool->getPlayingSources();

                auto end = std::remove_if(sources.begin(), sources.end(),
                                          [](SourceBase* source) { return !source->isPlaying(); });

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

                return audio::Channel::isPaused(this->channel) == false;
            }

            bool Source::isFinished() const
            {
                if (!this->valid)
                    return false;

                if (this->sourceType == TYPE_STREAM && (this->isLooping() || !this->decoder->isFinished()))
                    return false;

                return audio::Channel::isPlaying(this->channel) == false;
            }

            void Source::setMinVolume(float volume)
            {
                this->minVolume = volume;
            }

            float Source::getMinVolume() const
            {
                return this->minVolume;
            }

            void Source::setMaxVolume(float volume)
            {
                this->maxVolume = volume;
            }

            float Source::getMaxVolume() const
            {
                return this->maxVolume;
            }

            int Source::getChannelCount() const
            {
                return this->channels;
            }

            void Source::setVolume(float volume)
            {
                if (this->valid)
                    audio::Channel::setVolume(this->channel, volume);

                this->volume = volume;
            }

            float Source::getVolume() const
            {
                if (this->valid)
                    return audio::Channel::getVolume(this->channel);

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
                        offsetSeconds = offset / ((double)this->samplerate / this->channels);
                        break;
                    }
                    case UNIT_SECONDS:
                    default:
                    {
                        offsetSamples = (int)(offset * this->samplerate * this->channels);
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
                        offset = audio::Channel::getSampleOffset(this->channel);
                    else
                        offset = this->offsetSamples;
                }

                return (unit == UNIT_SAMPLES) ? offset : (offset / this->samplerate);
            }

            double Source::getDuration(Unit unit)
            {
                auto lock = this->pool->lock();

                switch (this->sourceType)
                {
                    case TYPE_STATIC:
                    {
                        const auto size     = this->staticBuffer->getSize();
                        const auto nsamples = ((size / this->channels) / (this->bitdepth / 8)) -
                                              (this->offsetSamples / this->channels);

                        return (unit == UNIT_SAMPLES) ? nsamples : (nsamples / this->samplerate);
                    }
                    case TYPE_STREAM:
                    {
                        double seconds = this->decoder->getDuration();
                        return (unit == UNIT_SAMPLES) ? (seconds * this->samplerate) : seconds;
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
                            if (buffer.isFinished())
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

            bool Source::queue(void* data, size_t length, int samplerate, int bitdepth, int channels)
            {
                if (this->sourceType != TYPE_QUEUE)
                    throw QueueTypeMismatchException();

                if (samplerate != this->samplerate || bitdepth != this->bitdepth ||
                    channels != this->channels)
                {
                    throw QueueFormatMismatchException();
                }

                if (length % (this->bitdepth / 8 * this->channels) != 0)
                    throw QueueMalformedLengthException(this->bitdepth / 8 * this->channels);

                if (length == 0)
                    return true;

                auto lock = this->pool->lock();

                if (this->unusedBuffers.empty())
                    return false;

                auto buffer = this->unusedBuffers.top();
                this->unusedBuffers.pop();

                buffer.prepare(data, 0, 0);

                if (this->valid)
                {
                }
                else
                    this->streamBuffers.push_back(buffer);

                return true;
            }

            void Source::prepareAtomic()
            {
                this->reset();

                switch (this->sourceType)
                {
                    case TYPE_STATIC:
                    {
                        auto& buffer = this->staticBuffer->getView(this->offsetSamples, this->channels);
                        audio::Channel::addBuffer(this->channel, buffer);

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

            void Source::teardownAtomic()
            {
                audio::Channel::stop(this->channel);

                switch (this->sourceType)
                {
                    case TYPE_STATIC:
                        break;
                    case TYPE_STREAM:
                    {
                        this->decoder->rewind();
                        audio::Channel::stop(this->channel);

                        break;
                    }
                    case TYPE_QUEUE:
                        audio::Channel::stop(this->channel);
                        break;
                    default:
                        break;
                }

                this->valid         = false;
                this->offsetSamples = 0;
            }

            int Source::streamAtomic(audio::Buffer& buffer, Decoder* decoder)
            {
                int decoded = std::max(decoder->decode(), 0);

                if (decoded > 0)
                {
                    // DigitalSound::getInstance().setLooping(buffer, this->looping);

                    const int samples = int((decoded / this->channels) / (this->bitdepth / 8));
                    buffer.prepare(decoder->getBuffer(), decoded, samples);
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
                        audio::Channel::addBuffer(this->channel, buffer);
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

                audio::Channel::pause(this->channel, true);
            }

            void Source::resumeAtomic()
            {
                if (!this->valid)
                    return;

                audio::Channel::pause(this->channel, false);
            }
        } // namespace dsp
    } // namespace audio
} // namespace love
