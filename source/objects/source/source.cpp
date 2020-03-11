#include "common/runtime.h"
#include "objects/source/source.h"

using namespace love;

love::Type Source::type("Source", &Object::type);

StaticDataBuffer::StaticDataBuffer(void * data, size_t size) : size(size)
{
    this->buffer = (s16 *)linearAlloc(size);
    memcpy(this->buffer, data, size);
}

StaticDataBuffer::~StaticDataBuffer()
{
    linearFree(this->buffer);
}

Source::Source(SoundData * sound) : sourceType(Source::TYPE_STATIC),
                                    sampleRate(sound->GetSampleRate()),
                                    channels(sound->GetChannelCount()),
                                    bitDepth(sound->GetBitDepth())
{

    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
    ndspChnSetRate(0, this->sampleRate);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);

    this->source = ndspWaveBuf();
    this->source.nsamples = sound->GetSampleCount();

    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()));
}

Source::Source(Decoder * decoder) : sourceType(Source::TYPE_STREAM),
                                    sampleRate(decoder->GetSampleRate()),
                                    channels(decoder->GetChannelCount()),
                                    bitDepth(decoder->GetBitDepth()),
                                    decoder(decoder),
                                    buffers(DEFAULT_BUFFERS)
{}

Source::Source(const Source & other)
{}

Source::~Source()
{
    this->Stop();
}

void Source::Reset()
{}

void Source::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            this->source.data_pcm16 = (s16 *)this->staticBuffer.Get()->GetBuffer();
            break;
        case TYPE_STREAM:
            break;
        case TYPE_QUEUE:
        default:
            break;
    }
}

int Source::StreamAtomic(void * buffer, Decoder * decoder)
{
    // int decoded = std::max(decoder->Decode(), 0);

    // if (decoded > 0)
    // {
    //     buffer = linearAlloc(decoded);
    //     memcpy(buffer, decoder->GetBuffer(), decoded);

    //     DSP_FlushDataCache(buffer, decoded);
    // }

    // return decoded;
    return 0;
}

bool Source::PlayAtomic()
{
    //this->source = source;
    this->PrepareAtomic();

    bool success = false;

    Result res = DSP_FlushDataCache(this->source.data_pcm16, this->staticBuffer->GetSize());

    if (R_SUCCEEDED(res))
    {
        ndspChnWaveBufAdd(0, &this->source);
        success = true;
    }

    if (this->sourceType == TYPE_STREAM)
    {
        this->valid = true;
        if (!this->IsPlaying())
            success = false;
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

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        ndspChnSetPaused(0, false);
}

bool Source::Play()
{
    if (!ndspChnIsPaused(0))
        return this->valid = this->PlayAtomic();

    this->ResumeAtomic();

    return this->valid = true;
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    return ndspChnIsPlaying(0);
}

void Source::Stop()
{
    if (!this->valid)
        return;

    ndspChnWaveBufClear(0);
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    return (ndspChnIsPlaying(0) == false);
}

Source * Source::Clone()
{
    return new Source(*this);
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

StringMap<Source::Type, Source::TYPE_MAX_ENUM> Source::types(Source::typeEntries, sizeof(Source::typeEntries));
