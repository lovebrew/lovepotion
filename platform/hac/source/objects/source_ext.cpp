#include <algorithm>
#include <objects/source_ext.hpp>

#include <utilities/driver/dsp_mem.hpp>
#include <utilities/pool/sources.hpp>

using namespace love;

using DSP = love::DSP<Console::HAC>;

template<>
Source<Console::HAC>::DataBuffer::DataBuffer(const void* data, size_t size) : size(size)
{
    this->buffer = (int16_t*)AudioMemory::Align(size, this->alignSize);
    std::memcpy(this->buffer, (int16_t*)data, size);

    armDCacheFlush(this->buffer, this->size);
}

template<>
Source<Console::HAC>::DataBuffer::~DataBuffer()
{
    AudioMemory::Free(this->buffer, this->alignSize);
}

Source<Console::HAC>::Source(AudioPool* pool, SoundData* soundData) :
    Source<>(TYPE_STATIC),
    pool(pool)
{
    this->sampleRate    = soundData->GetSampleRate();
    this->channels      = soundData->GetChannelCount();
    this->bitDepth      = soundData->GetBitDepth();
    this->samplesOffset = 0;

    std::fill_n(this->buffers, 2, WaveInfo {});

    this->staticBuffer = std::make_shared<DataBuffer>(soundData->GetData(), soundData->GetSize());
}

Source<Console::HAC>::Source(AudioPool* pool, Decoder* decoder) : Source<>(TYPE_STREAM), pool(pool)
{
    this->decoder       = decoder;
    this->sampleRate    = decoder->GetSampleRate();
    this->channels      = decoder->GetChannelCount();
    this->bitDepth      = decoder->GetBitDepth();
    this->bufferCount   = MAX_BUFFERS;
    this->samplesOffset = 0;

    std::fill_n(this->buffers, this->bufferCount, WaveInfo {});

    for (auto& info : this->buffers)
    {
        info.buffer.data_pcm16 = (int16_t*)AudioMemory::Align(decoder->GetSize(), info.alignedSize);
        info.buffer.state      = AudioDriverWaveBufState_Done;
    }
}

Source<Console::HAC>::Source(AudioPool* pool, int sampleRate, int bitDepth, int channels,
                             int buffers) :
    Source<>(TYPE_QUEUE),
    pool(pool)
{
    this->sampleRate    = sampleRate;
    this->channels      = channels;
    this->bitDepth      = bitDepth;
    this->bufferCount   = buffers;
    this->samplesOffset = 0;

    if (buffers < 1 || buffers > Source::MAX_BUFFERS)
        buffers = MAX_BUFFERS;

    std::fill_n(this->buffers, this->bufferCount, WaveInfo {});

    for (auto& info : this->buffers)
        info.buffer.state = AudioDriverWaveBufState_Done;
}

Source<Console::HAC>::Source(const Source& other) : Source<>(other.sourceType), pool(other.pool)
{
    this->staticBuffer  = other.staticBuffer;
    this->decoder       = nullptr;
    this->sampleRate    = other.sampleRate;
    this->channels      = other.channels;
    this->bitDepth      = other.bitDepth;
    this->bufferCount   = other.bufferCount;
    this->samplesOffset = other.samplesOffset;

    if (this->sourceType == TYPE_STREAM)
    {
        if (other.decoder.Get())
            this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
    }

    std::fill_n(this->buffers, this->bufferCount, WaveInfo {});

    for (size_t index = 0; index < this->bufferCount; index++)
    {
        if (this->sourceType == TYPE_STREAM)
        {
            this->buffers[index].buffer.data_pcm16 =
                (int16_t*)AudioMemory::Align(decoder->GetSize(), this->buffers[index].alignedSize);
        }

        this->buffers[index].buffer.state = AudioDriverWaveBufState_Done;
    }
}

Source<Console::HAC>::~Source()
{
    this->Stop();

    for (auto& info : this->buffers)
    {
        if (info.buffer.data_pcm16)
            AudioMemory::Free(info.buffer.data_pcm16, info.alignedSize);
    }
}

Source<Console::HAC>* Source<Console::HAC>::Clone()
{
    return new Source(*this);
}

bool Source<Console::HAC>::Play()
{
    uint8_t wasPlaying = false;

    {
        auto lock = this->pool->Lock();
        if (!this->pool->AssignSource(this, this->channel, wasPlaying))
            return this->valid = false;
    }

    if (!wasPlaying)
    {
        if (!(this->valid = this->PlayAtomic(this->buffers[0].buffer)))
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

void Source<Console::HAC>::Reset()
{
    ::DSP::Instance().ChannelReset(this->channel, this->channels, this->bitDepth, this->sampleRate);
}

void Source<Console::HAC>::Stop()
{
    if (!this->valid)
        return;

    auto lock = this->pool->Lock();
    this->pool->ReleaseSource(this);
}

void Source<Console::HAC>::Pause()
{
    auto lock = this->pool->Lock();

    if (this->pool->IsPlaying(this))
        this->PauseAtomic();
}

bool Source<Console::HAC>::IsPlaying() const
{
    return this->valid && !::DSP::Instance().IsChannelPaused(this->channel);
}

bool Source<Console::HAC>::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (this->IsLooping() || !this->decoder->IsFinished()))
        return false;

    if (this->sourceType == TYPE_STATIC)
        return this->buffers[0].buffer.state == AudioDriverWaveBufState_Done;

    return ::DSP::Instance().IsChannelPlaying(this->channel) == false;
}

bool Source<Console::HAC>::Update()
{
    if (!this->valid)
        return false;

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            return !this->IsFinished();
        case TYPE_STREAM:
        {
            if (this->IsFinished())
                return false;

            bool other = !this->current;
            if (this->buffers[other].buffer.state == AudioDriverWaveBufState_Done)
            {
                int decoded = this->StreamAtomic(this->buffers[other].buffer, this->decoder.Get());

                if (decoded == 0)
                    return false;

                ::DSP::Instance().ChannelAddBuffer(this->channel, &this->buffers[other].buffer);
                this->samplesOffset += this->buffers[other].buffer.start_sample_offset;

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

void Source<Console::HAC>::SetVolume(float volume)
{
    if (volume < this->GetMinVolume() || volume > this->GetMaxVolume())
        return;

    if (this->valid)
        ::DSP::Instance().ChannelSetVolume(this->channel, volume);

    this->volume = volume;
}

float Source<Console::HAC>::GetVolume() const
{
    if (this->valid)
        return ::DSP::Instance().ChannelGetVolume(this->channel);

    return this->volume;
}

void Source<Console::HAC>::Seek(double offset, Unit unit)
{
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

double Source<Console::HAC>::Tell(Unit unit)
{
    auto lock = this->pool->Lock();

    int offset = 0;

    if (this->valid)
    {
        if (this->sourceType == TYPE_STATIC)
            offset += ::DSP::Instance().ChannelGetSampleOffset(this->channel);
        else
            offset = this->samplesOffset;
    }

    if (unit == UNIT_SECONDS)
        return offset / (double)sampleRate;

    return offset;
}

double Source<Console::HAC>::GetDuration(Unit unit)
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

void Source<Console::HAC>::SetLooping(bool loop)
{
    if (this->sourceType == TYPE_QUEUE)
        throw QueueLoopingException();

    if (this->valid && this->sourceType == TYPE_STATIC)
        this->buffers[0].buffer.is_looping = loop;

    this->looping = loop;
}

/* todo */
bool Source<Console::HAC>::Queue(void* data, size_t length, int sampleRate, int bitDepth,
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

    return true;
}

int Source<Console::HAC>::GetFreeBufferCount() const
{
    if (this->sourceType == TYPE_STATIC)
        return 0;

    size_t count = 0;
    for (auto& info : this->buffers)
        count += (info.buffer.state == AudioDriverWaveBufState_Done) ? 1 : 0;

    return count;
}

void Source<Console::HAC>::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
        {
            const auto size = this->staticBuffer->GetSize();

            // clang-format off
            this->buffers[0].buffer.size = size;
            this->buffers[0].buffer.end_sample_offset   = (int)((size / this->channels) / (this->bitDepth / 8)) - (this->samplesOffset / this->channels);
            this->buffers[0].buffer.data_pcm16 = this->staticBuffer->GetBuffer() + (size_t)this->samplesOffset;
            // clang-format on

            this->buffers[0].buffer.is_looping = this->looping;

            break;
        }
        case TYPE_STREAM:
        {
            if (this->StreamAtomic(this->buffers[0].buffer, this->decoder.Get()) == 0)
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

int Source<Console::HAC>::StreamAtomic(AudioDriverWaveBuf& buffer, Decoder* decoder)
{
    int decoded = std::max(decoder->Decode(), 0);

    if (decoded > 0)
    {
        std::memcpy(buffer.data_pcm16, (int16_t*)decoder->GetBuffer(), decoded);

        buffer.size              = decoded;
        buffer.end_sample_offset = (int)((decoded / this->channels) / (this->bitDepth / 8));

        armDCacheFlush(buffer.data_pcm16, decoded);
    }

    if (decoder->IsFinished() && this->IsLooping())
        decoder->Rewind();

    return decoded;
}

/* todo */
void Source<Console::HAC>::TeardownAtomic()
{
    ::DSP::Instance().ChannelStop(this->channel);

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
        {
            this->decoder->Rewind();

            for (auto& info : this->buffers)
                info.buffer.state = AudioDriverWaveBufState_Done;

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

bool Source<Console::HAC>::PlayAtomic(AudioDriverWaveBuf& waveBuffer)
{
    this->PrepareAtomic();

    ::DSP::Instance().ChannelAddBuffer(this->channel, &waveBuffer);

    if (this->sourceType != TYPE_STREAM)
        this->samplesOffset = 0;

    if (this->sourceType == TYPE_STREAM)
        this->valid = true;

    return true;
}

void Source<Console::HAC>::StopAtomic()
{
    if (!this->valid)
        return;

    this->TeardownAtomic();
}

void Source<Console::HAC>::PauseAtomic()
{
    if (this->valid)
        ::DSP::Instance().ChannelPause(this->channel);
}

void Source<Console::HAC>::ResumeAtomic()
{
    if (this->valid)
        ::DSP::Instance().ChannelPause(this->channel, false);
}

bool Source<Console::HAC>::Play(const std::vector<Source*>& sources)
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

void Source<Console::HAC>::Stop(const std::vector<Source*>& sources)
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

void Source<Console::HAC>::Pause(const std::vector<Source*>& sources)
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
std::vector<Source<Console::HAC>*> Source<Console::HAC>::Pause(AudioPool* pool)
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

void Source<Console::HAC>::Stop(AudioPool* pool)
{
    std::vector<Source*> sources;

    {
        auto lock = pool->Lock();
        sources   = pool->GetPlayingSources();
    }

    Source::Stop(sources);
}

int Source<Console::HAC>::GetChannelCount() const
{
    return this->channels;
}
