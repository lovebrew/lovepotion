#include "common/runtime.h"
#include "objects/source/source.h"

#include "modules/audio/audio.h"

using namespace love;

love::Type Source::type("Source", &Object::type);

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))

Source::Source(SoundData * sound) : sourceType(Source::TYPE_STATIC),
                                    sampleRate(sound->GetSampleRate()),
                                    channels(sound->GetChannelCount()),
                                    bitDepth(sound->GetBitDepth())
{
    this->CreateWaveBuffer(sound);
    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()), Acquire::NORETAIN);
    this->channel = AudioModule()->GetOpenChannel();
}

Source::Source(Decoder * decoder) : sourceType(Source::TYPE_STREAM),
                                    sampleRate(decoder->GetSampleRate()),
                                    channels(decoder->GetChannelCount()),
                                    bitDepth(decoder->GetBitDepth()),
                                    decoder(decoder),
                                    buffers(DEFAULT_BUFFERS)
{
    this->CreateWaveBuffer(decoder);
    this->channel = AudioModule()->GetOpenChannel();
}

Source::Source(const Source & other) : sourceType(other.sourceType),
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
                                       staticBuffer(other.staticBuffer),
                                       buffers(other.buffers)
{
    if (this->sourceType == TYPE_STREAM)
    {
        if (other.decoder.Get())
        {
            this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
            this->CreateWaveBuffer(this->decoder);
        }
    }
    else
        AudioModule()->AddSourceToPool(this);

    this->channel = AudioModule()->GetOpenChannel();
}

Source::~Source()
{
    this->Stop();

    this->FreeBuffer();

    AudioModule()->FreeChannel(this->channel);
}

bool Source::Update()
{
    if (!this->valid)
        return false;

    switch(this->sourceType)
    {
        case TYPE_STATIC:
            return !this->IsFinished();
        case TYPE_STREAM:
        {
            if (!this->IsFinished())
                return this->_Update();

            return false;
        }
        case TYPE_QUEUE:
        default:
            break;
    }

    return false;
}

void Source::TeardownAtomic()
{
    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
            this->decoder->Rewind();
        case TYPE_QUEUE:
        default:
            break;
    }
}

void Source::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            this->sources[0].data_pcm16 = (s16 *)this->staticBuffer.Get()->GetBuffer();
            break;
        case TYPE_STREAM:
        {
            int decoded = this->StreamAtomic(this->sources[this->index].data_pcm16, this->decoder.Get());

            if (decoded == 0)
                break;

            int samples = (int)((decoded / this->channels) / (this->bitDepth / 8));
            this->_PrepareSamples(samples);

            if (this->decoder->IsFinished())
                break;

            break;
        }
        case TYPE_QUEUE:
        default:
            break;
    }
}

int Source::StreamAtomic(s16 * buffer, Decoder * decoder)
{
    int decoded = std::max(decoder->Decode(buffer), 0);

    if (decoded > 0)
        FlushAudioCache(buffer, decoded);

    if (this->decoder->IsFinished() && this->IsLooping())
        this->decoder->Rewind();

    return decoded;
}

bool Source::PlayAtomic()
{
    this->PrepareAtomic();

    bool success = false;
    LOG("Flush data_pcm16")
    if (this->sourceType != TYPE_STREAM)
        FlushAudioCache(this->sources[this->index].data_pcm16, this->staticBuffer->GetSize());
    LOG("Add WaveBuffer")
    this->AddWaveBuffer();
    LOG("Done")
    success = true;

    if (this->sourceType == TYPE_STREAM)
    {
        this->valid = true;
        svcSleepThread(5000000);

        if (!this->IsPlaying())
            success = false;
        else
            AudioModule()->AddSourceToPool(this);
    }

    if (!success)
    {
        this->valid = true;
        this->Stop();
    }

    if (this->sourceType != TYPE_STREAM)
        this->offsetSamples = 0;

    return success;
}

Source * Source::Clone()
{
    return new Source(*this);
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
