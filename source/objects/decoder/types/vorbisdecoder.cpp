#include "vorbisdecoder.h"

using namespace love;

/* libvorbis callbacks */

static int vorbisClose(void* data)
{
    /* Handled elsewhere */

    return 1;
}

static size_t vorbisRead(void* data, size_t byteSize, size_t readSize, void* source)
{
    size_t spaceUntilEOF  = 0;
    size_t actualSizeRead = 0;

    VorbisDecoder::OggFile* vorbisData = (VorbisDecoder::OggFile*)source;
    spaceUntilEOF                      = vorbisData->size - vorbisData->read;

    if ((readSize * byteSize) < spaceUntilEOF)
        actualSizeRead = (readSize * byteSize);
    else
        actualSizeRead = spaceUntilEOF;

    if (actualSizeRead)
    {
        memcpy(data, (const char*)vorbisData->data + vorbisData->read, actualSizeRead);
        vorbisData->read += (actualSizeRead);
    }

    return actualSizeRead;
}

static int vorbisSeek(void* source, ogg_int64_t offset, int whence)
{
    int64_t spaceUntilEOF;
    int64_t actualOffset;
    VorbisDecoder::OggFile* vorbisData;

    vorbisData = (VorbisDecoder::OggFile*)source;

    switch (whence)
    {
        case SEEK_SET:
            if (vorbisData->size >= offset)
                actualOffset = offset;
            else
                actualOffset = vorbisData->size;

            vorbisData->read = (int)actualOffset;
            break;
        case SEEK_CUR:
            spaceUntilEOF = vorbisData->size - vorbisData->read;

            if (offset < spaceUntilEOF)
                actualOffset = offset;
            else
                actualOffset = spaceUntilEOF;

            vorbisData->read += actualOffset;

            break;
        case SEEK_END:
            if (offset < 0)
                vorbisData->read = vorbisData->size + offset;
            else
                vorbisData->read = vorbisData->size;

            break;
        default:
            break;
    }

    return 0;
}

static long vorbisTell(void* source)
{
    VorbisDecoder::OggFile* vorbisData;
    vorbisData = (VorbisDecoder::OggFile*)source;

    return vorbisData->read;
}

/* VorbisDecoder */

VorbisDecoder::VorbisDecoder(Data* data, int bufferSize) : Decoder(data, bufferSize), duration(-2.0)
{
    this->callbacks.close_func = vorbisClose;
    this->callbacks.seek_func  = vorbisSeek;
    this->callbacks.read_func  = vorbisRead;
    this->callbacks.tell_func  = vorbisTell;

    this->file.data = (const char*)data->GetData();
    this->file.size = data->GetSize();
    this->file.read = 0;

    int success;
    if ((success = ov_open_callbacks(&this->file, &this->handle, NULL, 0, this->callbacks)) < 0)
        throw love::Exception("Could not read Ogg bitstream (error: %d).", success);

    this->info    = ov_info(&this->handle, -1);
    this->comment = ov_comment(&this->handle, -1);
}

VorbisDecoder::~VorbisDecoder()
{
    ov_clear(&this->handle);
}

bool VorbisDecoder::Accepts(const std::string& extension)
{
    static const std::string supported[] = { "ogg", "oga", "ogv", "" };

    for (size_t i = 0; !(supported[i].empty()); i++)
    {
        if (supported[i].compare(extension) == 0)
            return true;
    }

    return false;
}

Decoder* VorbisDecoder::Clone()
{
    return new VorbisDecoder(this->data.Get(), this->bufferSize);
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
