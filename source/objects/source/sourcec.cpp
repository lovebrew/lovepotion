#include "common/runtime.h"

#include "objects/source/sourcec.h"
#include "modules/audio/pool/pool.h"

using namespace love::common;

love::Type Source::type("Source", &Object::type);

Source::Source(Pool * pool, SoundData * sound) : sourceType(Source::TYPE_STATIC),
                                                 pool(pool),
                                                 sampleRate(sound->GetSampleRate()),
                                                 channels(sound->GetChannelCount()),
                                                 bitDepth(sound->GetBitDepth())
{
    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()), Acquire::NORETAIN);
}

Source::Source(Pool * pool, Decoder * decoder) : sourceType(Source::TYPE_STREAM),
                                                 pool(pool),
                                                 sampleRate(decoder->GetSampleRate()),
                                                 channels(decoder->GetChannelCount()),
                                                 bitDepth(decoder->GetBitDepth()),
                                                 decoder(decoder)
{}

Source::Source(const Source & other) : sourceType(other.sourceType),
                                       pool(other.pool),
                                       valid(false),
                                       volume(other.volume),
                                       looping(other.looping),
                                       minVolume(other.minVolume),
                                       maxVolume(other.maxVolume),
                                       offsetSamples(0),
                                       sampleRate(other.sampleRate),
                                       channels(other.channels),
                                       bitDepth(other.bitDepth),
                                       decoder(nullptr),
                                       staticBuffer(other.staticBuffer)
{
    if (this->sourceType == TYPE_STREAM && other.decoder.Get())
        this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
}

void Source::TeardownAtomic()
{
    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
            this->decoder->Rewind();
            this->InitializeStreamBuffers(this->decoder);
            break;
        case TYPE_QUEUE:
        default:
            break;
    }
}

bool Source::Play()
{
    thread::Lock lock = this->pool->Lock();

    bool wasPlaying;
    if (!this->pool->AssignSource(this, this->channel, wasPlaying))
        return valid = false;

    if (!wasPlaying)
        return this->valid = this->PlayAtomic();

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
            size_t size = this->staticBuffer->GetSize();
            size_t samples = (size / this->channels) / (this->bitDepth / 8);

            if (unit == UNIT_SAMPLES)
                return (double)samples;
            else
                return (double)samples / (double)sampleRate;
        }
        case TYPE_STREAM:
        {
            double seconds = this->decoder->GetDuration();

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
    int offsetSamples = 0;
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

/* CONSTANTS */

bool Source::GetConstant(const char * in, Type & out)
{
    return types.Find(in, out);
}

bool Source::GetConstant(Type in, const char  *& out)
{
    return types.Find(in, out);
}

std::vector<std::string> Source::GetConstants(Type)
{
    return types.GetNames();
}

StringMap<Source::Type, Source::TYPE_MAX_ENUM>::Entry Source::typeEntries[] =
{
    { "static", Source::TYPE_STATIC },
    { "stream", Source::TYPE_STREAM },
    { "queue",  Source::TYPE_QUEUE  },
};

bool Source::GetConstant(const char * in, Unit & out)
{
	return units.Find(in, out);
}

bool Source::GetConstant(Unit in, const char *& out)
{
	return units.Find(in, out);
}

std::vector<std::string> Source::GetConstants(Unit)
{
	return units.GetNames();
}

StringMap<Source::Type, Source::TYPE_MAX_ENUM> Source::types(Source::typeEntries, sizeof(Source::typeEntries));

StringMap<Source::Unit, Source::UNIT_MAX_ENUM>::Entry Source::unitEntries[] =
{
	{"seconds", Source::UNIT_SECONDS},
	{"samples", Source::UNIT_SAMPLES},
};

StringMap<Source::Unit, Source::UNIT_MAX_ENUM> Source::units(Source::unitEntries, sizeof(Source::unitEntries));
