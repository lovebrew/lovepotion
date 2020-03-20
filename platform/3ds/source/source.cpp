#include "common/runtime.h"
#include "objects/source/source.h"

#include "modules/audio/audio.h"

using namespace love;

StaticDataBuffer::StaticDataBuffer(void * data, size_t size)
{
    this->buffer.first = (s16 *)linearAlloc(size);
    this->buffer.second = size;

    memcpy(this->buffer.first, data, size);
}

StaticDataBuffer::~StaticDataBuffer()
{
    linearFree(this->buffer.first);
}

bool Source::_Update()
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

        int samples = (int)((decoded / this->channels) / (this->bitDepth / 8));

        this->sources[index].nsamples = samples;
        ndspChnWaveBufAdd(this->channel, &this->sources[index]);
    }

    return true;
}

void Source::_PrepareSamples(int samples)
{
    this->sources[this->index].nsamples = samples;
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
    {
        for (int i = 0; i < this->buffers; i++)
            linearFree(this->sources[i].data_pcm16);
    }
}

void Source::CreateWaveBuffer(SoundData * sound)
{
    this->sources[0] = waveBuffer();
    this->sources[0].nsamples = sound->GetSampleCount();
}

void Source::CreateWaveBuffer(Decoder * decoder)
{
    for (int i = 0; i < 2; i++)
    {
        this->sources[i] = waveBuffer();
        this->sources[i].nsamples = 0;
        this->sources[i].data_pcm16 = (s16 *)linearAlloc(decoder->GetSize());
    }
    this->sources[1].status = NDSP_WBUF_DONE;
}

void Source::SetLooping(bool shouldLoop)
{
    this->looping = shouldLoop;

    for (int i = 0; i < this->buffers; i ++)
        this->sources[i].looping = shouldLoop;
}

void Source::AddWaveBuffer()
{
    ndspChnWaveBufAdd(this->channel, &this->sources[this->index]);
}

void Source::SetVolume(float volume)
{
    this->volume = volume;

    float mix[12];
    memset(mix, 0, sizeof(mix));

    mix[0] = mix[1] = volume;

    ndspChnSetMix(this->channel, mix);
}

void Source::Reset()
{
    ndspChnReset(this->channel);

    u16 format = NDSP_FORMAT_STEREO_PCM16;

    switch (this->channels)
    {
        case 1:
            if (this->bitDepth == 8)
                format = NDSP_FORMAT_MONO_PCM8;
            else
                format = NDSP_FORMAT_MONO_PCM16;

            break;
        case 2:
            if (this->bitDepth == 8)
                format = NDSP_FORMAT_STEREO_PCM8;
            else
                format = NDSP_FORMAT_STEREO_PCM16;

            break;
        default:
            break;
    }

    ndspInterpType interpType = (this->channels == 2) ?
                                    NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR;

    ndspChnSetFormat(this->channel, format);
    ndspChnSetRate(this->channel, this->sampleRate);
    ndspChnSetInterp(this->channel, interpType);
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        ndspChnSetPaused(this->channel, false);
}

bool Source::Play()
{
    if (!ndspChnIsPaused(this->channel))
        return this->valid = this->PlayAtomic();

    this->ResumeAtomic();

    return this->valid = true;
}

void Source::Pause()
{
    ndspChnSetPaused(this->channel, true);
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    return this->sources[this->index].status == NDSP_WBUF_PLAYING;
}

void Source::Stop()
{
    if (!this->valid)
        return;

    ndspChnWaveBufClear(this->channel);
    this->TeardownAtomic();
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    return ndspChnIsPlaying(this->channel) == false;
}
