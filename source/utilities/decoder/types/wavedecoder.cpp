#include <common/exception.hpp>
#include <common/strongreference.hpp>

#include <utilities/decoder/types/wavedecoder.hpp>

using namespace love;

/* callbacks */

static wuff_sint32 read(void* source, wuff_uint8* buffer, size_t* size)
{
    auto stream     = (Stream*)source;
    size_t readSize = stream->Read(buffer, *size);

    *size = readSize;

    return WUFF_SUCCESS;
}

static wuff_sint32 seek(void* source, wuff_uint64 offset)
{
    auto stream = (Stream*)source;
    stream->Seek(offset, Stream::ORIGIN_BEGIN);

    return WUFF_SUCCESS;
}

static wuff_sint32 tell(void* source, wuff_uint64* offset)
{
    auto stream = (Stream*)source;
    *offset     = stream->Tell();

    return WUFF_SUCCESS;
}

/* end callbacks */

static wuff_callback callbacks { read, seek, tell };

WaveDecoder::WaveDecoder(Stream* stream, int bufferSize) : Decoder(stream, bufferSize)
{
    int status = wuff_open(&this->handle, &callbacks, stream);

    if (status < 0)
        throw love::Exception("Could not open WAVE.");

    try
    {
        status = wuff_stream_info(this->handle, &this->info);
        if (status < 0)
            throw love::Exception("Could not retrieve WAVE stream info.");

        if (this->info.channels > 2)
            throw love::Exception("WAVE Multichannel audio is not supported.");

        if (this->info.format != WUFF_FORMAT_PCM_U8 && this->info.format != WUFF_FORMAT_PCM_S16)
        {
            status = wuff_format(this->handle, WUFF_FORMAT_PCM_S16);

            if (status < 0)
                throw love::Exception("Could not set WAVE output format");
        }
    }
    catch (love::Exception&)
    {
        wuff_close(this->handle);
        throw;
    }
}

WaveDecoder::~WaveDecoder()
{
    wuff_close(this->handle);
}

Decoder* WaveDecoder::Clone()
{
    StrongReference<Stream> cloneStream(this->stream->Clone(), Acquire::NORETAIN);

    return new WaveDecoder(cloneStream, bufferSize);
}

int WaveDecoder::Decode()
{
    size_t size = 0;

    while (size < (size_t)this->bufferSize)
    {
        size_t bytes = this->bufferSize - size;
        int status   = wuff_read(this->handle, (wuff_uint8*)this->GetBuffer() + size, &bytes);

        if (status < 0)
            return 0;
        else if (bytes == 0)
        {
            this->eof = true;
            break;
        }
        size += bytes;
    }

    return size;
}

bool WaveDecoder::Seek(double position)
{
    int status = wuff_seek(this->handle, (wuff_uint64)(position * this->info.sample_rate));

    if (status < 0)
    {
        this->eof = false;
        return true;
    }

    return false;
}

bool WaveDecoder::Rewind()
{
    int status = wuff_seek(this->handle, 0);

    if (status >= 0)
    {
        this->eof = false;
        return true;
    }

    return false;
}

bool WaveDecoder::IsSeekable()
{
    return true;
}

int WaveDecoder::GetChannelCount() const
{
    return this->info.channels;
}

int WaveDecoder::GetBitDepth() const
{
    return this->info.bits_per_sample == 8 ? 8 : 16;
}

int WaveDecoder::GetSampleRate() const
{
    return this->info.sample_rate;
}

double WaveDecoder::GetDuration()
{
    return (double)this->info.length / (double)this->info.sample_rate;
}
