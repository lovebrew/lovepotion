#include <objects/source_ext.hpp>
#include <utilities/pool/sources.hpp>

#include <algorithm>
#include <utilities/log/logfile.h>

using namespace love;

using DSP = love::DSP<Console::CTR>;

template<>
Source<Console::CTR>::DataBuffer::DataBuffer(const void* data, size_t size) : size(size)
{
    this->buffer = (int16_t*)linearAlloc(size);
    std::memcpy(this->buffer, (int16_t*)data, size);

    DSP_FlushDataCache(this->buffer, this->size);
}

template<>
Source<Console::CTR>::DataBuffer::~DataBuffer()
{
    linearFree(this->buffer);
}

Source<Console::CTR>::Source(AudioPool* pool, SoundData* soundData) :
    Source<>(TYPE_STATIC),
    pool(pool),
    current(false),
    sampleRate(soundData->GetSampleRate()),
    channels(soundData->GetChannelCount()),
    bitDepth(soundData->GetBitDepth()),
    samplesOffset(0),
    channel(0)
{
    std::fill_n(this->buffers, 2, ndspWaveBuf {});

    this->staticBuffer = std::make_shared<DataBuffer>(soundData->GetData(), soundData->GetSize());
}

Source<Console::CTR>::Source(AudioPool* pool, Decoder* decoder) :
    Source<>(TYPE_STREAM),
    pool(pool),
    current(false),
    decoder(decoder),
    sampleRate(decoder->GetSampleRate()),
    channels(decoder->GetSampleRate()),
    bitDepth(decoder->GetBitDepth()),
    bufferCount(MAX_BUFFERS),
    samplesOffset(0),
    channel(0)
{
    std::fill_n(this->buffers, 2, ndspWaveBuf {});

    for (size_t index = 0; index < this->bufferCount; index++)
    {
        this->buffers[index].data_pcm16 = (int16_t*)linearAlloc(decoder->GetSize());
        this->buffers[index].status     = NDSP_WBUF_DONE;
    }
}

Source<Console::CTR>::Source(AudioPool* pool, int sampleRate, int bitDepth, int channels,
                             int buffers) :
    Source<>(TYPE_QUEUE),
    pool(pool),
    sampleRate(sampleRate),
    channels(channels),
    bitDepth(bitDepth),
    bufferCount(buffers),
    samplesOffset(0),
    channel(0)
{
    if (buffers < 1 || buffers > Source::MAX_BUFFERS)
        buffers = MAX_BUFFERS;

    std::fill_n(this->buffers, this->bufferCount, ndspWaveBuf {});

    for (size_t index = 0; index < this->bufferCount; index++)
        this->buffers[index].status = NDSP_WBUF_DONE;
}

Source<Console::CTR>::Source(const Source& other) :
    Source<>(other.sourceType),
    pool(other.pool),
    valid(false),
    current(false),
    staticBuffer(other.staticBuffer),
    decoder(nullptr),
    sampleRate(other.sampleRate),
    channels(other.channels),
    bitDepth(other.bitDepth),
    bufferCount(other.bufferCount),
    samplesOffset(other.samplesOffset)
{
    if (this->sourceType == TYPE_STREAM)
    {
        if (other.decoder.Get())
            this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
    }

    std::fill_n(this->buffers, this->bufferCount, ndspWaveBuf {});

    for (size_t index = 0; index < this->bufferCount; index++)
    {
        if (this->sourceType == TYPE_STREAM)
            this->buffers[index].data_pcm16 = (int16_t*)linearAlloc(this->decoder->GetSize());

        this->buffers[index].status = NDSP_WBUF_DONE;
    }
}

Source<Console::CTR>::~Source()
{
    this->Stop();

    for (auto& buffer : this->buffers)
    {
        if (buffer.data_pcm16)
            linearFree(buffer.data_pcm16);
    }
}

Source<Console::CTR>* Source<Console::CTR>::Clone()
{
    return new Source(*this);
}

bool Source<Console::CTR>::Play()
{
    uint8_t wasPlaying = false;

    {
        auto lock = this->pool->Lock();
        if (!this->pool->AssignSource(this, this->channel, wasPlaying))
            return this->valid = false;
    }

    if (!wasPlaying)
    {
        if (!(this->valid = this->PlayAtomic(this->buffers[0])))
            return false;

        this->ResumeAtomic();

        {
            auto lock = this->pool->Lock();
            this->pool->AddSource(this, this->channel);
        }

        return this->valid;
    }

    this->ResumeAtomic();

    return this->valid = true;
}

void Source<Console::CTR>::Reset()
{
    ::DSP::Instance().ChannelReset(this->channel, this->channels, this->bitDepth, this->sampleRate);
}

void Source<Console::CTR>::Stop()
{
    if (!this->valid)
        return;

    auto lock = this->pool->Lock();
    this->pool->ReleaseSource(this);
}

void Source<Console::CTR>::Pause()
{
    auto lock = this->pool->Lock();

    if (this->pool->IsPlaying(this))
        this->PauseAtomic();
}

bool Source<Console::CTR>::IsPlaying() const
{
    return this->valid && !::DSP::Instance().IsChannelPaused(this->channel);
}

bool Source<Console::CTR>::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (this->IsLooping() || !this->decoder->IsFinished()))
        return false;

    if (this->sourceType == TYPE_STATIC)
        return this->buffers[0].status == NDSP_WBUF_DONE;

    return ::DSP::Instance().IsChannelPlaying(this->channel) == false;
}

bool Source<Console::CTR>::Update()
{
    if (!this->valid)
        return false;

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            return !this->IsFinished();
        case TYPE_STREAM:
        {
            bool other = !this->current;
            if (this->buffers[other].status == NDSP_WBUF_DONE)
            {
                int decoded = this->StreamAtomic(this->buffers[other], this->decoder.Get());

                if (decoded == 0)
                    return false;

                ::DSP::Instance().ChannelAddBuffer(this->channel, &this->buffers[other]);
                this->samplesOffset += this->buffers[other].nsamples;

                this->current = !this->current;
            }
            return true;
        }
        case TYPE_QUEUE:
            break;
        default:
            break;
    }

    return false;
}

void Source<Console::CTR>::SetVolume(float volume)
{
    if (volume < this->GetMinVolume() || volume > this->GetMaxVolume())
        return;

    if (this->valid)
        ::DSP::Instance().ChannelSetVolume(this->channel, volume);

    this->volume = volume;
}

float Source<Console::CTR>::GetVolume() const
{
    if (this->valid)
        return ::DSP::Instance().ChannelGetVolume(this->channel);

    return this->volume;
}

/* todo */
void Source<Console::CTR>::Seek(double offset, Unit unit)
{
    // auto lock = this->pool->Lock();

    int offsetSamples    = 0;
    double offsetSeconds = 0.0f;

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
            offsetSeconds = offset;
            offsetSamples = (int)(offset * sampleRate * this->channels);
        }
    }

    bool wasPlaying = this->IsPlaying();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
        {
            if (this->valid)
                this->Stop();

            this->samplesOffset = offsetSamples;

            if (wasPlaying)
                this->Play();

            break;
        }
        case TYPE_STREAM:
        {
            if (this->valid)
                this->Stop();

            this->decoder->Seek(offsetSeconds);

            if (wasPlaying)
                this->Play();

            break;
        }
        case TYPE_QUEUE:
        {
            /* todo */
        }
        default:
            break;
    }

    if (wasPlaying && (this->sourceType == TYPE_STREAM && !this->IsPlaying()))
    {
        this->Stop();

        if (this->IsLooping())
            this->Play();

        return;
    }

    this->samplesOffset = offsetSamples;
}

/* todo */
double Source<Console::CTR>::Tell(Unit unit)
{
    auto lock = this->pool->Lock();

    int offset = 0;

    if (this->valid)
        offset += ::DSP::Instance().ChannelGetSampleOffset(this->channel);

    if (unit == UNIT_SECONDS)
        return offset / (double)sampleRate / this->channels;

    return offset;
}

/* todo */
double Source<Console::CTR>::GetDuration(Unit unit)
{
    auto lock = this->pool->Lock();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
        {
            size_t size    = this->staticBuffer->GetSize();
            size_t samples = (size / this->channels) / (this->bitDepth / 8);

            if (unit == UNIT_SAMPLES)
                return (double)samples;

            return (double)samples / (double)sampleRate;
        }
        case TYPE_STREAM:
        {
            /* vorbisidec 1.2.1 uses ms, not sec, convert */
            double seconds = this->decoder->GetDuration() / 1000.0;

            if (unit == UNIT_SECONDS)
                return seconds;

            return seconds * decoder->GetSampleRate();
        }
        case TYPE_QUEUE:
        {
            /* todo */
            break;
        }
        default:
            return 0.0;
    }

    return 0.0;
}

/* todo */
void Source<Console::CTR>::SetLooping(bool loop)
{
    if (this->sourceType == TYPE_QUEUE)
        throw QueueLoopingException();

    if (this->valid && this->sourceType == TYPE_STATIC)
        this->buffers[0].looping = loop;

    this->looping = loop;
}

/* todo */
bool Source<Console::CTR>::IsLooping() const
{
    return this->looping;
}

/* todo */
bool Source<Console::CTR>::Queue(void* data, size_t length, int sampleRate, int bitDepth,
                                 int channels)
{
    if (this->sourceType != TYPE_QUEUE)
        throw QueueTypeMismatchException();

    if (sampleRate != this->sampleRate || bitDepth != this->bitDepth || channels != this->channels)
        throw QueueFormatMismatchException();

    if (length % (bitDepth / 8 * channels) != 0)
        throw QueueMalformedLengthException(bitDepth / 8 * channels);

    if (length == 0)
        return true;

    // ndspWaveBuf buffer {};
    // buffer.data_pcm16 = (int16_t*)linearAlloc(length);
    // std::memcpy(buffer.data_pcm16, data, length);

    // buffer.nsamples = (int)((length / this->channels) / (this->bitDepth / 8));
    // this->queue.push(buffer);

    return true;
}

int Source<Console::CTR>::GetFreeBufferCount() const
{
    if (this->sourceType == TYPE_STATIC)
        return 0;

    size_t count = 0;
    for (auto& buffer : this->buffers)
        count += (buffer.status == NDSP_WBUF_DONE) ? 1 : 0;

    return count;
}

void Source<Console::CTR>::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
        {
            const auto size = this->staticBuffer->GetSize();

            // clang-format off
            this->buffers[0].nsamples   = (int)((size / this->channels) / (this->bitDepth / 8)) - (this->samplesOffset / this->channels);
            this->buffers[0].data_pcm16 = this->staticBuffer->GetBuffer() + (size_t)this->samplesOffset;
            // clang-format on

            break;
        }
        case TYPE_STREAM:
        {
            if (this->StreamAtomic(this->buffers[0], this->decoder.Get()) == 0)
                break;

            if (this->decoder->IsFinished())
                break;

            break;
        }
        case TYPE_QUEUE:
            break; /* todo */
        default:
            break;
    }
}

int Source<Console::CTR>::StreamAtomic(ndspWaveBuf& buffer, Decoder* decoder)
{
    int decoded = std::max(decoder->Decode(), 0);

    if (decoded > 0)
    {
        std::memcpy(buffer.data_pcm16, (int16_t*)decoder->GetBuffer(), decoded);
        DSP_FlushDataCache(buffer.data_pcm16, decoded);
    }

    if (decoder->IsFinished() && this->IsLooping())
        decoder->Rewind();

    buffer.nsamples = (int)((decoded / this->channels) / (this->bitDepth / 8));

    return decoded;
}

/* todo */
void Source<Console::CTR>::TeardownAtomic()
{
    ::DSP::Instance().ChannelStop(this->channel);

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
        {
            this->decoder->Rewind();

            std::fill_n(this->buffers, this->bufferCount, ndspWaveBuf {});

            for (auto& buffer : this->buffers)
                buffer.status = NDSP_WBUF_DONE;

            break;
        }
        case TYPE_QUEUE:
            break; /* todo */
        default:
            break;
    }

    this->valid         = false;
    this->samplesOffset = 0;
}

bool Source<Console::CTR>::PlayAtomic(ndspWaveBuf& waveBuffer)
{
    this->PrepareAtomic();

    ::DSP::Instance().ChannelAddBuffer(this->channel, &waveBuffer);

    if (this->sourceType != TYPE_STREAM)
        this->samplesOffset = 0;

    if (this->sourceType == TYPE_STREAM)
        this->valid = true;

    return true;
}

void Source<Console::CTR>::StopAtomic()
{
    if (!this->valid)
        return;

    this->TeardownAtomic();
}

void Source<Console::CTR>::PauseAtomic()
{
    if (this->valid)
        ::DSP::Instance().ChannelPause(this->channel);
}

void Source<Console::CTR>::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        ::DSP::Instance().ChannelPause(this->channel, false);
}

bool Source<Console::CTR>::Play(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return true;

    auto* pool = ((Source*)sources[0])->pool;
    auto lock  = pool->Lock();

    std::vector<uint8_t> wasPlaying(sources.size());
    std::vector<size_t> channels(sources.size());

    for (size_t index = 0; index < sources.size(); index++)
    {
        if (!pool->AssignSource((Source*)sources[index], channels[index], wasPlaying[index]))
        {
            for (size_t j = 0; j < index; j++)
            {
                if (!wasPlaying[j])
                    pool->ReleaseSource((Source*)sources[index], false);
            }

            return false;
        }
    }

    std::vector<Source*> toPlay;
    toPlay.reserve(sources.size());

    for (size_t index = 0; index < sources.size(); index++)
    {
        if (wasPlaying[index] && sources[index]->IsPlaying())
            continue;

        if (!wasPlaying[index])
        {
            auto* source    = (Source*)sources[index];
            source->channel = channels[index];

            source->PrepareAtomic();
        }

        toPlay.push_back(sources[index]);
    }

    for (auto& _source : toPlay)
    {
        auto* source = (Source*)_source;

        if (source->sourceType != TYPE_STREAM)
            source->samplesOffset = 0;

        if (!(_source->valid = _source->Play()))
            return false;

        pool->AddSource(_source, source->channel);
    }

    return true;
}

void Source<Console::CTR>::Stop(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return;

    auto* pool = ((Source*)sources[0])->pool;
    auto lock  = pool->Lock();

    std::vector<Source*> toStop;
    toStop.reserve(sources.size());

    for (auto& _source : sources)
    {
        auto* source = (Source*)_source;

        if (source->valid)
            toStop.push_back(source);
    }

    for (auto& _source : toStop)
    {
        auto* source = (Source*)_source;

        if (source->valid)
            source->TeardownAtomic();

        pool->ReleaseSource(source, false);
    }
}

void Source<Console::CTR>::Pause(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return;

    auto lock = ((Source*)sources[0])->pool->Lock();

    for (auto& _source : sources)
    {
        auto* source = (Source*)_source;

        if (source->valid)
            source->PauseAtomic();
    }
}

/* todo */
std::vector<Source<Console::CTR>*> Source<Console::CTR>::Pause(AudioPool* pool)
{
    std::vector<Source*> sources;

    {
        auto lock = pool->Lock();
        sources   = pool->GetPlayingSources();

        auto end = std::remove_if(sources.begin(), sources.end(),
                                  [](Source* source) { return !source->IsPlaying(); });

        sources.erase(end, sources.end());
    }

    Source::Pause(sources);

    return sources;
}

void Source<Console::CTR>::Stop(AudioPool* pool)
{
    std::vector<Source*> sources;

    {
        auto lock = pool->Lock();
        sources   = pool->GetPlayingSources();
    }

    Source::Stop(sources);
}

int Source<Console::CTR>::GetChannelCount() const
{
    return this->channels;
}
