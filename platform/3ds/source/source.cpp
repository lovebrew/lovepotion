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

void Source::CreateWaveBuffer(waveBuffer * buffer, size_t size, size_t nsamples)
{
    buffer->nsamples = nsamples;
}

void Source::AddWaveBuffer()
{
    ndspChnWaveBufAdd(0, &this->sources[this->index]);
}

void Source::Reset()
{
    ndspChnReset(0);

    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
    ndspChnSetRate(0, this->sampleRate);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
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

void Source::Pause()
{
    ndspChnSetPaused(0, true);
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    return ndspChnIsPlaying(0) == true;
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

    return ndspChnIsPlaying(0) == false;
}
