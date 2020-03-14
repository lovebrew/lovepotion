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
    this->source = waveBuffer();
    this->CreateWaveBuffer(&this->source, sound->GetSize(), sound->GetSampleCount());
    this->staticBuffer.Set(new StaticDataBuffer(sound->GetData(), sound->GetSize()), Acquire::NORETAIN);
}

Source::Source(Decoder * decoder) : sourceType(Source::TYPE_STREAM),
                                    sampleRate(decoder->GetSampleRate()),
                                    channels(decoder->GetChannelCount()),
                                    bitDepth(decoder->GetBitDepth()),
                                    decoder(decoder),
                                    buffers(DEFAULT_BUFFERS)
{
    this->source = waveBuffer();
    this->CreateWaveBuffer(&this->source, 0, 0);

    for (int i = 0; i < this->buffers; i++)
    {
        s16 * buffer = (s16 *)linearAlloc(decoder->GetSize());

        if (buffer)
            this->unusedBuffers.push(buffer);
        else
        {
            this->buffers = i;
            break;
        }
    }
}

Source::Source(const Source & other)
{}

Source::~Source()
{
    this->Stop();

    if (this->sourceType != TYPE_STATIC)
    {
        while (!this->streamBuffers.empty())
        {
            linearFree(this->streamBuffers.front());
            this->streamBuffers.pop();
        }

        while (!this->unusedBuffers.empty())
        {
            linearFree(this->unusedBuffers.top());
            this->unusedBuffers.pop();
        }
    }
}

void Source::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            this->source.data_pcm16 = (s16 *)this->staticBuffer.Get()->GetBuffer();
            break;
        case TYPE_STREAM:
            ndspChnSetPaused(0, true);

            while (!this->unusedBuffers.empty())
            {
                auto buffer = this->unusedBuffers.top();

                /* Set up the buffers with the Decoder's data */

                int decoded = this->StreamAtomic(buffer, this->decoder.Get());

                if (decoded == 0)
                    break;

                /* LOVE calls alSourceQueueBuffers which .. queues them */
                /* ndsp automatically queues buffers when you add them to a channel */
                /* so we paused the channel before doing any of this */
                this->source.nsamples = ((decoded / this->channels) / (this->bitDepth / 8));
                this->source.data_pcm16 = buffer;

                DSP_FlushDataCache(this->source.data_pcm16, decoded);

                ndspChnWaveBufAdd(0, &this->source);

                this->unusedBuffers.pop();

                if (this->decoder->IsFinished())
                    break;
            }
            break;
        case TYPE_QUEUE:
        default:
            break;
    }
}

int Source::StreamAtomic(void * buffer, Decoder * decoder)
{
    int decoded = std::max(decoder->Decode(), 0);

    if (decoded > 0)
    {
        memcpy(buffer, decoder->GetBuffer(), decoded);
    }

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
    {
        FlushAudioCache(this->source.data_pcm16, this->staticBuffer->GetSize());

        this->AddWaveBuffer();
    }
    else
    {
        ndspChnSetPaused(0, false);
    }

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
