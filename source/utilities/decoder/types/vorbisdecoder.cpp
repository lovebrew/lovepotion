#include <common/exception.hpp>

#include <utilities/decoder/types/vorbisdecoder.hpp>

using namespace love;

/* callbacks */

static int close(void*)
{
    return 1;
}

static size_t read(void* buffer, size_t byteSize, size_t readSize, void* source)
{
    auto stream = (Stream*)source;

    return stream->Read(buffer, byteSize * readSize);
}

static int seek(void* source, ogg_int64_t offset, int whence)
{
    auto stream = (Stream*)source;

    return stream->Seek(offset, *Stream::seekOrigins.Find(whence)) ? 0 : -1;
}

static long tell(void* source)
{
    auto stream = (Stream*)source;

    return stream->Tell();
}

/* end callbacks */

VorbisDecoder::VorbisDecoder(Stream* stream, int bufferSize) :
    Decoder(stream, bufferSize),
    duration(-2.0)
{
    ov_callbacks callbacks {};
    callbacks.close_func = close;
    callbacks.seek_func  = seek;
    callbacks.read_func  = read;
    callbacks.tell_func  = tell;

    if (ov_open_callbacks(stream, &this->handle, nullptr, 0, callbacks) < 0)
        throw love::Exception("Could not read Ogg bitstream");

    this->info = ov_info(&this->handle, -1);
}

VorbisDecoder::~VorbisDecoder()
{
    ov_clear(&this->handle);
}

Decoder* VorbisDecoder::Clone()
{
    StrongReference<Stream> cloneStream(this->stream->Clone(), Acquire::NORETAIN);

    return new VorbisDecoder(cloneStream, bufferSize);
}

int VorbisDecoder::Decode()
{
    int size      = 0;
    int bitStream = 0;

    while (size < this->bufferSize)
    {
        int length = this->bufferSize - size;

        long result = ov_read(&this->handle, (char*)this->GetBuffer() + size, length, &bitStream);

        if (result == OV_HOLE)
            continue;
        else if (result <= OV_EREAD)
            return -1;
        else if (result == 0)
        {
            this->eof = true;
            break;
        }
        else if (result > 0)
            size += result;
    }

    return size;
}

bool VorbisDecoder::Seek(double position)
{
    int result = 0;

    // Avoid ov_time_seek (which calls ov_pcm_seek) when seeking to 0, to avoid
    // a bug in libvorbis <= 1.3.4 when seeking to PCM 0 in multiplexed streams.
    if (position <= 0.000001)
        result = ov_raw_seek(&this->handle, 0);
    else
        result = ov_time_seek(&this->handle, position);

    if (result == 0)
    {
        this->eof = false;
        return true;
    }

    return false;
}

bool VorbisDecoder::Rewind()
{
    int result = ov_raw_seek(&this->handle, 0);

    if (result == 0)
    {
        this->eof = false;
        return true;
    }

    return false;
}

bool VorbisDecoder::IsSeekable()
{
    long result = ov_seekable(&this->handle);

    return (result != 0);
}

int VorbisDecoder::GetChannelCount() const
{
    return this->info->channels;
}

int VorbisDecoder::GetBitDepth() const
{
    return 16;
}

int VorbisDecoder::GetSampleRate() const
{
    return this->info->rate;
}

double VorbisDecoder::GetDuration()
{
    if (this->duration == -2.0)
    {
        this->duration = ov_time_total(&this->handle, -1);

        if (this->duration == OV_EINVAL || this->duration < 0.0)
            this->duration = -1.0;
    }

    return duration;
}
