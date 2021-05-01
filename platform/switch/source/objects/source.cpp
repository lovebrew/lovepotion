#include "objects/source/source.h"
#include "modules/audio/audio.h"

#include "driver/audiodrv.h"
#include "pools/audiopool.h"

using namespace love;

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))

StaticDataBuffer::StaticDataBuffer(void* data, size_t size)
{
    std::pair<void*, size_t> buff = AudioPool::MemoryAlign(size);

    if (buff.first)
    {
        memcpy(buff.first, data, size);

        this->buffer = { (s16*)buff.first, buff.second };
    }
}

StaticDataBuffer::~StaticDataBuffer()
{
    AudioPool::MemoryFree(this->buffer);
}

/* SOURCE IMPLEMENTATION */

Source::Source(Pool* pool, SoundData* sound) : common::Source(pool, sound)
{
    this->sources[0]                     = AudioDriverWaveBuf();
    this->sources[0].start_sample_offset = 0;
    this->sources[0].size                = sound->GetSize();
    this->sources[0].end_sample_offset   = sound->GetSampleCount();
}

Source::Source(Pool* pool, Decoder* decoder) : common::Source(pool, decoder)
{
    this->InitializeStreamBuffers(decoder);
}

Source::Source(const Source& other) : common::Source(other)
{
    this->InitializeStreamBuffers(this->decoder.Get());
}

love::Source* Source::Clone()
{
    return new Source(*this);
}

Source::~Source()
{
    this->Stop();
    this->FreeBuffer();
}

void Source::InitializeStreamBuffers(Decoder* decoder)
{
    if (!this->sourceBuffer)
    {
        auto aligned = AudioPool::MemoryAlign(decoder->GetSize() * 2);

        this->sourceBuffer = aligned.first;
        this->souceBufferSize = aligned.second;
    }

    for (size_t i = 0; i < MAX_BUFFERS; i++)
    {
        auto buffer = (s16*)(((size_t)this->sourceBuffer) + i * decoder->GetSize());

        this->sources[i] = AudioDriverWaveBuf { .data_pcm16          = buffer,
                                                .size                = 0,
                                                .start_sample_offset = 0,
                                                .state = AudioDriverWaveBufState_Done };
    }
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
        AudioPool::MemoryFree(std::make_pair(this->sourceBuffer, this->souceBufferSize));
}

double Source::GetSampleOffset()
{
    double offset = 0;

    AudioModule()->GetDriver()->LockFunction([this, &offset](AudioDriver* driver) {
        offset = audrvVoiceGetPlayedSampleCount(driver, this->channel);
    });

    return offset;
}

void Source::SetVolume(float volume)
{
    AudioModule()->GetDriver()->LockFunction([this, volume](AudioDriver* driver) {
        audrvVoiceSetVolume(driver, this->channel, volume);
    });

    this->volume = volume;
}

void Source::Reset()
{
    AudioModule()->GetDriver()->LockFunction([this](AudioDriver* driver) {
        audrvVoiceInit(driver, this->channel, this->channels, PcmFormat_Int16, this->sampleRate);
        audrvVoiceSetDestinationMix(driver, this->channel, AUDREN_FINAL_MIX_ID);

        audrvVoiceSetMixFactor(driver, this->channel, 1.0f, 0, 0);
        audrvVoiceSetMixFactor(driver, this->channel, 1.0f, 0, 1);
    });
}

bool Source::Update()
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

            for (int which = 0; which < Source::MAX_BUFFERS; which++)
            {
                if (this->sources[which].state == AudioDriverWaveBufState_Done)
                {
                    int decoded = this->StreamAtomic(which);

                    if (decoded == 0)
                        return false;

                    AudioModule()->GetDriver()->LockFunction([this, which](AudioDriver* driver) {
                        audrvVoiceAddWaveBuf(driver, this->channel, &this->sources[which]);
                        audrvVoiceStart(driver, this->channel);
                    });
                }
            }

            return true;
        }
        case TYPE_QUEUE:
            break;
        case TYPE_MAX_ENUM:
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
            this->sources[0].data_pcm16 = (s16*)this->staticBuffer.Get()->GetBuffer();
            break;
        case TYPE_STREAM:
        {
            if (this->StreamAtomic(0) == 0)
                break;

            if (this->decoder->IsFinished())
                break;

            break;
        }
        case TYPE_QUEUE:
        default:
            break;
    }
}

int Source::StreamAtomic(size_t which)
{
    auto buffer = this->sources[which].data_pcm16;
    int decoded = std::max(decoder->Decode(buffer), 0);

    if (decoded > 0)
        armDCacheFlush(buffer, decoded);

    if (this->decoder->IsFinished() && this->IsLooping())
        this->decoder->Rewind();

    this->sources[which].size = decoded;
    this->sources[which].end_sample_offset =
        (int)((decoded / this->channels) / (this->bitDepth / 8));

    return decoded;
}

/* IS IT DOING STUFF */

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    bool sourcePlaying = false;

    AudioModule()->GetDriver()->LockFunction([ this, &sourcePlaying ](AudioDriver * driver)
    {
        sourcePlaying = audrvVoiceIsPlaying(driver, this->channel);
    });

    return this->valid && sourcePlaying;
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    bool finished = false;

    AudioModule()->GetDriver()->LockFunction([this, &finished](AudioDriver* driver) {
        finished = (audrvVoiceIsPlaying(driver, this->channel) == false);
    });

    return finished;
}

/* ATOMIC STATES */

bool Source::PlayAtomic()
{
    this->PrepareAtomic();

    if (this->sourceType != TYPE_STREAM) /* flush the armD data cache */
        armDCacheFlush(this->sources[0].data_pcm16, this->staticBuffer->GetSize());

    /* add the initial wavebuffer */
    AudioModule()->GetDriver()->LockFunction([this](AudioDriver* driver) {
        audrvVoiceAddWaveBuf(driver, this->channel, &this->sources[0]);
        audrvVoiceStart(driver, this->channel);
    });

    if (this->sourceType != TYPE_STREAM)
        this->offsetSamples = 0;

    if (this->sourceType == TYPE_STREAM)
        this->valid = true;

    return true;
}

void Source::PauseAtomic()
{
    AudioModule()->GetDriver()->LockFunction([this](AudioDriver* driver) {
        if (this->valid)
            audrvVoiceSetPaused(driver, this->channel, true);
    });
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
    {
        AudioModule()->GetDriver()->LockFunction(
            [this](AudioDriver* driver) { audrvVoiceSetPaused(driver, this->channel, false); });
    }
}

void Source::StopAtomic()
{
    AudioModule()->GetDriver()->LockFunction([this](AudioDriver* driver) {
        if (!this->valid)
            return;

        audrvVoiceStop(driver, this->channel);
        this->TeardownAtomic();
    });
}

void Source::SetLooping(bool should)
{
    this->looping = should;
    if (this->valid && this->sourceType == TYPE_STATIC)
        this->sources[0].is_looping = should;
}
