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
    this->sources[0] = waveBuffer();
    this->sources[0].nsamples = sound->GetSampleCount();

    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()), Acquire::NORETAIN);
}

Source::Source(Decoder * decoder) : sourceType(Source::TYPE_STREAM),
                                    sampleRate(decoder->GetSampleRate()),
                                    channels(decoder->GetChannelCount()),
                                    bitDepth(decoder->GetBitDepth()),
                                    decoder(decoder),
                                    buffers(DEFAULT_BUFFERS)
{
    for (int i = 0; i < 2; i++)
    {
        this->sources[i] = waveBuffer();
        this->sources[i].nsamples = 0;
        this->sources[i].data_pcm16 = (s16 *)linearAlloc(decoder->GetSize());
    }
    this->sources[1].status = NDSP_WBUF_DONE;

    AudioModule()->AddSourceToPool(this);
}

Source::Source(const Source & other)
{}

Source::~Source()
{
    this->Stop();

    if (this->sourceType != TYPE_STATIC)
    {

    }
}

bool Source::Update()
{
    if (!this->valid)
        return false;

    switch(this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
        {
            int index = 0;
            if (this->sources[index].status != NDSP_WBUF_DONE)
            {
                if (this->sources[index + 1].status == NDSP_WBUF_DONE)
                    index = 1;
            }

            if (this->sources[index].status == NDSP_WBUF_DONE)
            {
                int decoded = this->StreamAtomic(this->sources[index].data_pcm16, this->decoder.Get());

                if (decoded == 0)
                    return false;

                this->sources[index].nsamples = (int)((decoded / this->channels) / (this->bitDepth / 8));
                ndspChnWaveBufAdd(0, &this->sources[index]);
            }
        }
        case TYPE_QUEUE:
        default:
            break;
    }

    return false;
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

            this->sources[this->index].nsamples = (int)((decoded / this->channels) / (this->bitDepth / 8));

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

    if (decoder->IsFinished() && this->looping)
    {}

    return decoded;
}

bool Source::PlayAtomic()
{
    //this->source = source;
    this->PrepareAtomic();

    bool success = false;

    if (this->sourceType != TYPE_STREAM)
        FlushAudioCache(this->sources[this->index].data_pcm16, this->staticBuffer->GetSize());

    this->AddWaveBuffer();

    success = true;

    if (this->sourceType == TYPE_STREAM)
    {
        this->valid = true;

        // if (this->IsPlaying() == false)
        //     success = false;
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
