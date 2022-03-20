#include "objects/source/sourcec.h"
#include "modules/audio/pool/pool.h"

#include "common/bidirectionalmap.h"
#include "objects/source/source.h"

using namespace love::common;

love::Type Source::type("Source", &Object::type);

Source::Source(Pool* pool, SoundData* sound) :
    sourceType(Source::TYPE_STATIC),
    sampleRate(sound->GetSampleRate()),
    channels(sound->GetChannelCount()),
    bitDepth(sound->GetBitDepth()),
    pool(pool)
{
    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()),
                           Acquire::NORETAIN);
}

Source::Source(Pool* pool, Decoder* decoder) :
    sourceType(Source::TYPE_STREAM),
    sourceBuffer(nullptr),
    sampleRate(decoder->GetSampleRate()),
    channels(decoder->GetChannelCount()),
    bitDepth(decoder->GetBitDepth()),
    pool(pool),
    decoder(decoder)
{}

Source::Source(const Source& other) :
    sourceType(other.sourceType),
    sourceBuffer(nullptr),
    valid(false),
    volume(other.volume),
    looping(other.looping),
    minVolume(other.minVolume),
    maxVolume(other.maxVolume),
    offsetSamples(0),
    sampleRate(other.sampleRate),
    channels(other.channels),
    bitDepth(other.bitDepth),
    pool(other.pool),
    decoder(nullptr),
    staticBuffer(other.staticBuffer)
{
    if (this->sourceType == TYPE_STREAM && other.decoder.Get())
        this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
}

void Source::TeardownAtomic()
{
    this->ClearChannel();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
            this->decoder->Rewind();
            this->InitializeStreamBuffers(this->decoder.Get());
            break;
        case TYPE_QUEUE:
        default:
            break;
    }

    this->valid         = false;
    this->offsetSamples = 0;
}

bool Source::Play()
{
    char wasPlaying;

    {
        thread::Lock lock = this->pool->Lock();
        if (!this->pool->AssignSource(this, this->channel, wasPlaying))
            return this->valid = false;
    }

    if (!wasPlaying)
    {
        if (!(this->valid = this->PlayAtomic()))
            return false;

        this->ResumeAtomic();

        {
            thread::Lock lock = this->pool->Lock();
            this->pool->AddSource(this, this->channel);
        }

        return this->valid;
    }

    this->ResumeAtomic();

    return this->valid = true;
}

void Source::Stop()
{
    if (!this->valid)
        return;

    thread::Lock lock = pool->Lock();
    this->pool->ReleaseSource(this);
}

void Source::Pause()
{
    thread::Lock lock = pool->Lock();

    if (pool->IsPlaying(this))
        this->PauseAtomic();
}

Source::Type Source::GetType() const
{
    return this->sourceType;
}

int Source::GetChannelCount() const
{
    return this->channels;
}

float Source::GetMinVolume() const
{
    return this->minVolume;
}

float Source::GetMaxVolume() const
{
    return this->maxVolume;
}

float Source::GetVolume() const
{
    return this->volume;
}

int Source::GetFreeBufferCount() const
{
    int total = 0;

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            return 0;
        case TYPE_STREAM:
            // for (int i = 0; i < 2; i++)
            // {
            //     if (this->sources[i].status == NDSP_WBUF_DONE)
            //         total += 1;
            // }
            return total;
        case TYPE_QUEUE:
        default:
            break;
    }

    return total;
}

double Source::GetDuration(Source::Unit unit)
{
    switch (this->sourceType)
    {
        case TYPE_STATIC:
        {
            size_t size    = this->staticBuffer->GetSize();
            size_t samples = (size / this->channels) / (this->bitDepth / 8);

            if (unit == UNIT_SAMPLES)
                return (double)samples;
            else
                return (double)samples / (double)sampleRate;
        }
        case TYPE_STREAM:
        {
            /* vorbisidec 1.2.1 uses ms, not sec, convert */
            double seconds = this->decoder->GetDuration() / 1000;

            if (unit == UNIT_SECONDS)
                return seconds;
            else
                return seconds * this->decoder->GetSampleRate();
        }
        case TYPE_QUEUE:
        default:
            break;
    }

    return -1;
}

void Source::Seek(double offset, Source::Unit unit)
{
    int offsetSamples    = 0;
    double offsetSeconds = 0.0;

    switch (unit)
    {
        case UNIT_SAMPLES:
            offsetSamples = (int)offset;
            offsetSeconds = offset / (double)this->sampleRate;
            break;
        case UNIT_SECONDS:
        default:
            offsetSeconds = offset;
            offsetSamples = (int)(offset * this->sampleRate);
            break;
    }

    bool wasPlaying = this->IsPlaying();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            if (this->valid)
            {
                /* TODO: offset static buffer somehow */
                offsetSamples = offsetSeconds = 0;
            }
            break;
        case TYPE_STREAM:
            if (this->valid)
                this->Stop();

            this->decoder->Seek(offsetSeconds);

            if (wasPlaying)
                this->Play();

            break;
        case TYPE_QUEUE:
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

    this->offsetSamples = offsetSamples;
}

void Source::SetMinVolume(float volume)
{
    this->minVolume = volume;
}

void Source::SetMaxVolume(float volume)
{
    this->maxVolume = volume;
}

bool Source::IsLooping() const
{
    return this->looping;
}

double Source::Tell(Source::Unit unit)
{
    thread::Lock lock = this->pool->Lock();

    double offset = 0;

    if (this->valid)
        offset = this->offsetSamples + this->GetSampleOffset();

    if (unit == UNIT_SECONDS)
        return offset / (double)this->sampleRate;
    else
        return offset;
}

bool Source::Play(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return true;

    Pool* pool        = ((Source*)sources[0])->pool;
    thread::Lock lock = pool->Lock();

    std::vector<char> wasPlaying(sources.size());
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

    std::vector<common::Source*> toPlay;
    toPlay.reserve(sources.size());

    for (size_t index = 0; index < sources.size(); index++)
    {
        if (wasPlaying[index] && sources[index]->IsPlaying())
            continue;

        if (!wasPlaying[index])
        {
            Source* source  = (Source*)sources[index];
            source->channel = channels[index];

            toPlay.push_back(source);
        }
        else
        {
            Source* source = (Source*)sources[index];
            source->ResumeAtomic();
        }
    }

    for (auto& _source : toPlay)
    {
        Source* source = (Source*)_source;

        if (source->sourceType != TYPE_STREAM)
            source->offsetSamples = 0;

        if (!(_source->valid = _source->PlayAtomic()))
            return false;

        pool->AddSource(_source, source->channel);
    }

    return true;
}

void Source::Stop(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return;

    Pool* pool        = ((Source*)sources[0])->pool;
    thread::Lock lock = pool->Lock();

    std::vector<common::Source*> toStop;
    toStop.reserve(sources.size());

    for (auto& _source : sources)
    {
        Source* source = (Source*)_source;
        if (source->valid)
            toStop.push_back(source);
    }

    for (auto& _source : toStop)
    {
        Source* source = (Source*)_source;
        if (source->valid)
            source->TeardownAtomic();

        pool->ReleaseSource(source, false);
    }
}

void Source::Stop(Pool* pool)
{
    std::vector<Source*> sources;

    {
        thread::Lock lock = pool->Lock();
        sources           = pool->GetPlayingSources();
    }

    common::Source::Stop(sources);
}

void Source::Pause(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return;

    thread::Lock lock = ((Source*)sources[0])->pool->Lock();

    for (auto& _source : sources)
    {
        Source* source = (Source*)_source;
        if (source->valid)
            source->PauseAtomic();
    }
}

std::vector<Source*> Source::Pause(Pool* pool)
{
    std::vector<Source*> sources;

    {
        thread::Lock lock = pool->Lock();
        sources           = pool->GetPlayingSources();

        auto newEnd = std::remove_if(sources.begin(), sources.end(),
                                     [](Source* source) { return !source->IsPlaying(); });

        sources.erase(newEnd, sources.end());
    }

    Source::Pause(sources);

    return sources;
}

// clang-format off
constexpr auto types = BidirectionalMap<>::Create(
    "static", Source::Type::TYPE_STATIC,
    "stream", Source::Type::TYPE_STREAM,
    "queue",  Source::Type::TYPE_QUEUE
);

constexpr auto units = BidirectionalMap<>::Create(
    "seconds", Source::Unit::UNIT_SECONDS,
    "samples", Source::Unit::UNIT_SAMPLES
);
// clang-format on

/* Type Constants */

bool Source::GetConstant(const char* in, Type& out)
{
    return types.Find(in, out);
}

bool Source::GetConstant(Type in, const char*& out)
{
    return types.ReverseFind(in, out);
}

std::vector<const char*> Source::GetConstants(Type)
{
    return types.GetNames();
}

/* Unit Constants */

bool Source::GetConstant(const char* in, Unit& out)
{
    return units.Find(in, out);
}

bool Source::GetConstant(Unit in, const char*& out)
{
    return units.ReverseFind(in, out);
}

std::vector<const char*> Source::GetConstants(Unit)
{
    return units.GetNames();
}
