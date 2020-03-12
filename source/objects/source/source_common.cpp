#include "common/runtime.h"
#include "objects/source/source.h"

#include "modules/audio/audio.h"

using namespace love;

love::Type Source::type("Source", &Object::type);

Source::Source(SoundData * sound) : sourceType(Source::TYPE_STATIC),
                                    sampleRate(sound->GetSampleRate()),
                                    channels(sound->GetChannelCount()),
                                    bitDepth(sound->GetBitDepth())
{
    this->source = this->CreateWaveBuffer(sound->GetSize(), sound->GetSampleCount());
    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()), Acquire::NORETAIN);
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

void Source::PrepareAtomic()
{
    LOG("Reset")
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

    FlushAudioCache(this->source.data_pcm16, this->staticBuffer->GetSize());

    this->AddWaveBuffer();
    success = true;

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
