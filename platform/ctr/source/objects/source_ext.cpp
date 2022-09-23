#include <objects/source_ext.hpp>
#include <utilities/pool/sources.hpp>

#include <algorithm>

using namespace love;

using DSP = love::DSP<Console::CTR>;

Source<Console::CTR>::Source(AudioPool* pool, SoundData* soundData) :
    Source<>(TYPE_STATIC),
    pool(pool),
    current(false),
    sampleRate(soundData->GetSampleRate()),
    channels(soundData->GetChannelCount()),
    bitDepth(soundData->GetBitDepth())
{
    this->buffers = std::make_unique<ndspWaveBuf[]>(1);

    this->buffers[this->current].data_pcm16 = (int16_t*)soundData->GetData();
    this->buffers[this->current].nsamples   = soundData->GetSampleCount();
}

Source<Console::CTR>::Source(AudioPool* pool, Decoder* decoder) :
    Source<>(TYPE_STREAM),
    pool(pool),
    current(false),
    sampleRate(decoder->GetSampleRate()),
    channels(decoder->GetSampleRate()),
    bitDepth(decoder->GetBitDepth()),
    decoder(decoder)
{
    this->buffers = std::make_unique<ndspWaveBuf[]>(MAX_BUFFERS);

    for (size_t index = 0; index < Source::MAX_BUFFERS; index++)
        this->buffers[index].status = NDSP_WBUF_DONE;
}

Source<Console::CTR>::Source(AudioPool* pool, int sampleRate, int bitDepth, int channels,
                             int buffers) :
    Source<>(TYPE_QUEUE),
    pool(pool),
    sampleRate(sampleRate),
    channels(channels),
    bitDepth(bitDepth),
    bufferCount(buffers)
{
    if (buffers < 1 || buffers > Source::MAX_BUFFERS)
        buffers = MAX_BUFFERS;

    this->buffers = std::make_unique<ndspWaveBuf[]>(this->bufferCount);

    for (size_t index = 0; index < this->bufferCount; index++)
        this->buffers[index].status = NDSP_WBUF_DONE;
}

Source<Console::CTR>::Source(const Source& other) :
    Source<>(other.sourceType),
    pool(other.pool),
    valid(false),
    sampleRate(other.sampleRate),
    channels(other.channels),
    bitDepth(other.bitDepth),
    decoder(nullptr),
    bufferCount(other.bufferCount)
{
    if (this->sourceType == TYPE_STREAM)
    {
        if (other.decoder.Get())
            this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
    }

    if (this->sourceType != TYPE_STATIC)
    {
        this->buffers = std::make_unique<ndspWaveBuf[]>(this->bufferCount);

        for (size_t index = 0; index < this->bufferCount; index++)
            this->buffers[index].status = NDSP_WBUF_DONE;
    }
}

Source<Console::CTR>::~Source()
{
    this->Stop();
}

Source<Console::CTR>* Source<Console::CTR>::Clone()
{
    return new Source(*this);
}

bool Source<Console::CTR>::Play()
{
    uint8_t wasPlaying = false;

    {
        auto lock = this->pool->Lock();
        if (!this->pool->AssignSource(this, this->channel, wasPlaying))
            return this->valid = false;
    }

    if (!wasPlaying)
    {
        if (!(this->valid = this->PlayAtomic()))
            return false;
    }

    this->ResumeAtomic();

    return this->valid = true;
}

/* todo */
void Source<Console::CTR>::Reset()
{}

void Source<Console::CTR>::Stop()
{
    if (!this->valid)
        return;

    auto lock = this->pool->Lock();
    this->pool->ReleaseSource(this);
}

void Source<Console::CTR>::Pause()
{
    auto lock = this->pool->Lock();

    if (this->pool->IsPlaying(this))
        this->PauseAtomic();
}

bool Source<Console::CTR>::IsPlaying() const
{
    if (!this->valid)
        return false;

    return ::DSP::Instance().IsChannelPlaying(this->channel);
}

bool Source<Console::CTR>::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (this->IsLooping() || !this->decoder->IsFinished()))
        return false;

    if (this->sourceType == TYPE_STATIC)
        return this->buffers[0].status == NDSP_WBUF_DONE;

    return ::DSP::Instance().IsChannelPlaying(this->channel) == false;
}

bool Source<Console::CTR>::Update()
{
    if (!this->valid)
        return false;

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
            break;
        case TYPE_QUEUE:
            break;
        default:
            break;
    }

    return false;
}

void Source<Console::CTR>::SetVolume(float volume)
{
    if (!this->valid)
        return;

    ::DSP::Instance().ChannelSetVolume(this->channel, volume);
}

float Source<Console::CTR>::GetVolume() const
{
    if (!this->valid)
        return 0.0f;

    return ::DSP::Instance().ChannelGetVolume(this->channel);
}

/* todo */
void Source<Console::CTR>::Seek(double offset, Unit unit)
{}

/* todo */
double Source<Console::CTR>::Tell(Unit unit)
{}

/* todo */
double Source<Console::CTR>::GetDuration(Unit unit)
{}

/* todo */
void Source<Console::CTR>::SetLooping(bool loop)
{}

/* todo */
bool Source<Console::CTR>::IsLooping() const
{}

/* todo */
bool Source<Console::CTR>::Queue(void* data, size_t length, int sampleRate, int bitDepth,
                                 int channels)
{}

/* todo */
int Source<Console::CTR>::GetFreeBufferCount() const
{}

/* todo */
void Source<Console::CTR>::PrepareAtomic()
{}

/*todo */
int Source<Console::CTR>::StreamAtomic(ndspWaveBuf& buffer, Decoder* decoder)
{}

/* todo */
void Source<Console::CTR>::TeardownAtomic()
{}

/* todo */
bool Source<Console::CTR>::PlayAtomic()
{}

/* todo */
void Source<Console::CTR>::StopAtomic()
{}

/* todo */
void Source<Console::CTR>::PauseAtomic()
{}

/* todo */
void Source<Console::CTR>::ResumeAtomic()
{}

/* todo */
bool Source<Console::CTR>::Play(const std::vector<Source*>& sources)
{}

/* todo */
void Source<Console::CTR>::Stop(const std::vector<Source*>& sources)
{}

/* todo */
void Source<Console::CTR>::Pause(const std::vector<Source*>& sources)
{}

/* todo */
std::vector<Source<Console::CTR>*> Source<Console::CTR>::Pause(AudioPool* pool)
{}

/* todo */
void Source<Console::CTR>::Stop(AudioPool* pool)
{}

int Source<Console::CTR>::GetChannelCount() const
{
    return this->channels;
}
