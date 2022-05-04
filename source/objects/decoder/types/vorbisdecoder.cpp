#include "vorbisdecoder.h"

using namespace love;

/* libvorbis callbacks */

static int vorbisClose(void*)
{
    /* Handled elsewhere */

    return 1;
}

static size_t vorbisRead(void* data, size_t byteSize, size_t sizeToRead, void* source)
{
    Stream* stream = (Stream*)source;

    return stream->Read(data, byteSize * sizeToRead);
}

static int vorbisSeek(void* source, ogg_int64_t offset, int whence)
{
    Stream* stream            = (Stream*)source;
    Stream::SeekOrigin origin = Stream::SEEK_ORIGIN_BEGIN;

    Stream::GetConstant(whence, origin);

    return stream->Seek(offset, origin) ? 0 : -1;
}

static long vorbisTell(void* source)
{
    Stream* stream = (Stream*)source;

    return (long)stream->Tell();
}

/* VorbisDecoder */

VorbisDecoder::VorbisDecoder(Stream* stream, int bufferSize) :
    Decoder(stream, bufferSize),
    duration(-2.0)
{
    ov_callbacks callbacks;

    callbacks.close_func = vorbisClose;
    callbacks.seek_func  = vorbisSeek;
    callbacks.read_func  = vorbisRead;
    callbacks.tell_func  = vorbisTell;

    int success;
    if ((success = ov_open_callbacks(this->stream, &this->handle, NULL, 0, callbacks)) < 0)
        throw love::Exception("Could not read Ogg bitstream (error: %d).", success);

    this->info = ov_info(&this->handle, -1);
}

VorbisDecoder::~VorbisDecoder()
{
    ov_clear(&this->handle);
}

Decoder* VorbisDecoder::Clone()
{
    return new VorbisDecoder(this->stream->Clone(), this->bufferSize);
}

int VorbisDecoder::Probe(Stream* stream)
{
    char header[VorbisDecoder::HEADER_SIZE];

    if (stream->Read(header, VorbisDecoder::HEADER_SIZE) >= 0x04)
    {
        if (memcmp(header, VorbisDecoder::HEADER_TAG, 4) == 0)
            return 60;
    }

    return 1;
}

int VorbisDecoder::Decode()
{
    return this->Decode((s16*)this->buffer);
}

int VorbisDecoder::Decode(s16* buffer)
{
    int size      = 0;
    int bitstream = 0;

    while (size < this->bufferSize)
    {
        long result =
            ov_read(&this->handle, (char*)buffer + size, this->bufferSize - size, &bitstream);

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

bool VorbisDecoder::Seek(double seek)
{
    int result = 0;

    // Avoid ov_time_seek (which calls ov_pcm_seek) when seeking to 0, to avoid
    // a bug in libvorbis <= 1.3.4 when seeking to PCM 0 in multiplexed streams.
    if (seek < 0.000001)
        result = ov_raw_seek(&this->handle, 0);
    else
        result = ov_time_seek(&this->handle, seek);

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
    return (int)this->info->rate;
}

double VorbisDecoder::GetDuration()
{
    // only recalculate if not done
    if (this->duration == -2.0)
    {
        this->duration = ov_time_total(&this->handle, -1);

        if (this->duration == OV_EINVAL || this->duration < 0.0)
            this->duration = -1.0;
    }

    return duration;
}
