#include <objects/source_ext.hpp>
#include <utilities/pool/sources.hpp>

#include <algorithm>
#include <utilities/log/logfile.h>

using namespace love;

using DSP = love::DSP<Console::CTR>;

Source<Console::CTR>::Source(AudioPool* pool, SoundData* soundData) :
    Source<>(TYPE_STATIC),
    pool(pool),
    current(false),
    sampleRate(soundData->GetSampleRate()),
    channels(soundData->GetChannelCount()),
    bitDepth(soundData->GetBitDepth()),
    samplesOffset(0),
    channel(0)
{
    this->staticBuffer.Set(
        new DSP<Console::CTR>::DataBuffer(soundData->GetData(), soundData->GetSize()),
        Acquire::NORETAIN);

    std::fill_n(this->buffers, 2, ndspWaveBuf {});

    this->buffers[this->current].nsamples = soundData->GetSampleCount();
}

Source<Console::CTR>::Source(AudioPool* pool, Decoder* decoder) :
    Source<>(TYPE_STREAM),
    pool(pool),
    current(false),
    decoder(decoder),
    sampleRate(decoder->GetSampleRate()),
    channels(decoder->GetSampleRate()),
    bitDepth(decoder->GetBitDepth()),
    samplesOffset(0),
    channel(0)
{
    std::fill_n(this->buffers, 2, ndspWaveBuf {});

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
    bufferCount(buffers),
    samplesOffset(0),
    channel(0)
{
    if (buffers < 1 || buffers > Source::MAX_BUFFERS)
        buffers = MAX_BUFFERS;

    std::fill_n(this->buffers, this->bufferCount, ndspWaveBuf {});

    for (size_t index = 0; index < this->bufferCount; index++)
        this->buffers[index].status = NDSP_WBUF_DONE;
}

Source<Console::CTR>::Source(const Source& other) :
    Source<>(other.sourceType),
    pool(other.pool),
    valid(false),
    decoder(nullptr),
    sampleRate(other.sampleRate),
    channels(other.channels),
    bitDepth(other.bitDepth),
    bufferCount(other.bufferCount),
    samplesOffset(other.samplesOffset)
{
    if (this->sourceType == TYPE_STREAM)
    {
        if (other.decoder.Get())
            this->decoder.Set(other.decoder->Clone(), Acquire::NORETAIN);
    }

    if (this->sourceType != TYPE_STATIC)
    {
        std::fill_n(this->buffers, this->bufferCount, ndspWaveBuf {});

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
        if (!(this->valid = this->PlayAtomic(this->buffers[0])))
            return false;

        this->ResumeAtomic();

        {
            auto lock = this->pool->Lock();
            this->pool->AddSource(this, this->channel);
        }

        return this->valid;
    }

    this->ResumeAtomic();

    return this->valid = true;
}

void Source<Console::CTR>::Reset()
{
    ::DSP::Instance().ChannelReset(this->channel, this->channels, this->bitDepth, this->sampleRate);
}

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
    return this->valid && !::DSP::Instance().IsChannelPaused(this->channel);
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
            return !this->IsFinished();
        case TYPE_STREAM:
        {
            bool other = !this->current;
            if (this->buffers[other].status == NDSP_WBUF_DONE)
            {
                this->samplesOffset += this->buffers[other].nsamples;
                int decoded = this->StreamAtomic(this->buffers[other], this->decoder.Get());

                if (decoded == 0)
                    return false;

                ::DSP::Instance().ChannelAddBuffer(this->channel, &this->buffers[other]);
            }
            return true;
        }
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
{
    return false;
}

/* todo */
bool Source<Console::CTR>::Queue(void* data, size_t length, int sampleRate, int bitDepth,
                                 int channels)
{}

/* todo */
int Source<Console::CTR>::GetFreeBufferCount() const
{}

/* todo */
void Source<Console::CTR>::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
        {
            // clang-format off
            this->buffers[this->current].data_pcm16 = this->staticBuffer->GetBuffer();
            auto result = DSP_FlushDataCache(this->buffers[this->current].data_pcm16, this->staticBuffer->GetSize());
            LOG("DSP_FlushDataCache: %d", R_SUCCEEDED(result));
            // clang-format on
            break;
        }
        case TYPE_STREAM:
        {
            if (this->StreamAtomic(this->buffers[0], this->decoder.Get()) == 0)
                break;

            if (this->decoder->IsFinished())
                break;

            break;
        }
        case TYPE_QUEUE:
            break; /* todo */
        default:
            break;
    }

    /* todo: seek to offset */
}

int Source<Console::CTR>::StreamAtomic(ndspWaveBuf& buffer, Decoder* decoder)
{
    int decoded = std::max(decoder->Decode(), 0);

    if (decoded > 0)
    {
        std::copy_n((int16_t*)decoder->GetBuffer(), decoded, buffer.data_pcm16);
        DSP_FlushDataCache(buffer.data_pcm16, decoded);
    }

    if (decoder->IsFinished() && this->IsLooping())
        decoder->Rewind();

    buffer.nsamples = (int)((decoded / this->channels) / (this->bitDepth / 8));

    return decoded;
}

/* todo */
void Source<Console::CTR>::TeardownAtomic()
{
    ::DSP::Instance().ChannelStop(this->channel);

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            break;
        case TYPE_STREAM:
        {
            this->decoder->Rewind();

            std::fill_n(this->buffers, this->bufferCount, ndspWaveBuf {});

            for (auto& buffer : this->buffers)
            {
                buffer.status     = NDSP_WBUF_DONE;
                buffer.data_pcm16 = (int16_t*)linearAlloc(this->decoder->GetSize());
            }

            break;
        }
        case TYPE_QUEUE:
            break; /* todo */
        default:
            break;
    }

    this->valid         = false;
    this->samplesOffset = 0;
}

bool Source<Console::CTR>::PlayAtomic(ndspWaveBuf& waveBuffer)
{
    this->PrepareAtomic();

    ::DSP::Instance().ChannelAddBuffer(this->channel, &waveBuffer);

    if (this->sourceType != TYPE_STREAM)
        this->samplesOffset = 0;

    if (this->sourceType == TYPE_STREAM)
        this->valid = true;

    return true;
}

void Source<Console::CTR>::StopAtomic()
{
    if (!this->valid)
        return;

    this->TeardownAtomic();
}

void Source<Console::CTR>::PauseAtomic()
{
    if (this->valid)
        ::DSP::Instance().ChannelPause(this->channel);
}

void Source<Console::CTR>::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        ::DSP::Instance().ChannelPause(this->channel, false);
}

/* todo */
bool Source<Console::CTR>::Play(const std::vector<Source*>& sources)
{
    if (sources.size() == 0)
        return true;

    auto* pool = ((Source*)sources[0])->pool;
    auto lock  = pool->Lock();

    std::vector<uint8_t> wasPlaying(sources.size());
    std::vector<size_t> channels(sources.size());
}

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
