#include "mp3decoder.h"

using namespace love;

/* Handled by Decoder */
static void cleanup_callback(void*)
{}

static ssize_t read_callback(void* source, void* buffer, size_t count)
{
    Stream* stream = (Stream*)source;
    int64_t read   = stream->Read(buffer, count);

    return std::max<ssize_t>(0, read);
}

static off_t seek_callback(void* source, off_t offset, int whence)
{
    Stream* stream            = (Stream*)source;
    Stream::SeekOrigin origin = Stream::SEEK_ORIGIN_BEGIN;

    Stream::GetConstant(whence, origin);

    stream->Seek(offset, origin);

    return stream->Tell();
}

bool MP3Decoder::inited = false;

MP3Decoder::MP3Decoder(Stream* stream, int bufferSize) :
    Decoder(stream, bufferSize),
    handle(0),
    channels(MPG123_STEREO),
    duration(-2.0)
{
    int ret = 0;

    if (!inited)
    {
        ret = mpg123_init();

        if (ret != MPG123_OK)
            throw love::Exception("Could not initialize mpg123.");

        inited = (ret == MPG123_OK);
    }

    this->handle = mpg123_new(nullptr, nullptr);
    if (this->handle == nullptr)
        throw love::Exception("Could not create mpg123 decoder.");

    mpg123_param(this->handle, MPG123_ADD_FLAGS, MPG123_QUIET, 0);

    try
    {
        ret = mpg123_replace_reader_handle(this->handle, &read_callback, &seek_callback,
                                           &cleanup_callback);

        if (ret != MPG123_OK)
            throw love::Exception("Could not set mpg123 decoder callbacks.");

        ret = mpg123_open_handle(this->handle, this->stream);

        if (ret != MPG123_OK)
            throw love::Exception("Could not open mpg123 decoder.");

        long rate = 0;
        ret       = mpg123_getformat(this->handle, &rate, &this->channels, nullptr);

        if (ret == MPG123_ERR)
            throw love::Exception("Could not get mpg123 stream information.");

        /* in LOVE, they have no idea why this is a thing */
        if (this->channels == 0)
            this->channels = 2;

        long channelType = (this->channels == 2) ? MPG123_FORCE_STEREO : MPG123_MONO_MIX;
        mpg123_param(this->handle, MPG123_FLAGS, channelType, 0);

        mpg123_format_none(this->handle);
        mpg123_format(this->handle, rate, this->channels, MPG123_ENC_SIGNED_16);

        this->sampleRate = (int)rate;

        struct mpg123_frameinfo info;
        ret = mpg123_info(this->handle, &info);

        if (ret != MPG123_OK)
            throw love::Exception("Could not read mp3 data.");
    }
    catch (love::Exception& e)
    {
        mpg123_delete(this->handle);
        throw;
    }
}

MP3Decoder::~MP3Decoder()
{
    mpg123_delete(this->handle);
}

void MP3Decoder::Quit()
{
    if (inited)
        mpg123_exit();
}

Decoder* MP3Decoder::Clone()
{
    return new MP3Decoder(this->stream->Clone(), bufferSize);
}

int MP3Decoder::Probe(Stream* stream)
{
    uint8_t header[MP3Decoder::MIN_HEADER_SIZE];

    if (stream->Read(header, MP3Decoder::MIN_HEADER_SIZE) >= 10)
    {
        if (memcmp(header, MP3Decoder::ID3V1_TAG, 3) == 0)
        {
            stream->Seek(MP3Decoder::ID3V1_SIZE, Stream::SEEK_ORIGIN_BEGIN);
            if (stream->Read(header, 0x04) < 4)
                return 0;
        }
        else if (memcmp(header, MP3Decoder::ID3V2_TAG, 3) == 0)
        {
            size_t size = size_t(header[9] & 0x7F) | (size_t(header[8] & 0x7F) << 0x07);
            size |= (size_t(header[7] & 0x7F) << 0x0E) | (size_t(header[6] & 0x7F) << 0x15);

            stream->Seek(0xA + size, Stream::SEEK_ORIGIN_BEGIN);

            if (stream->Read(header, 0x04) < 4)
                return 0;
        }
    }
    else
        return 0;

    if ((header[0] == 0xFF) && (((header[1] >> 0x05) & 0x07) == 0x07))
    {
        if (((header[1] >> 0x03) & 0x03) == 0x01)
            return 0;

        if (((header[1] >> 0x01) & 0x03) == 0x00)
            return 0;

        if (((header[2] >> 0x04) & 0x0F) == 0x0F)
            return 0;

        if ((header[3] & 0x03) == 0x02)
            return 0;

        return 0x4B;
    }

    return 1;
}

int MP3Decoder::Decode()
{
    return this->Decode((s16*)this->buffer);
}

int MP3Decoder::Decode(s16* buffer)
{
    int size = 0;

    while (size < bufferSize && !this->eof)
    {
        size_t bytes = 0;

        int result = mpg123_read(this->handle, (unsigned char*)buffer + size,
                                 this->bufferSize - size, &bytes);

        switch (result)
        {
            case MPG123_NEED_MORE:
            case MPG123_NEW_FORMAT:
            case MPG123_OK:
                size += bytes;
                continue;
            case MPG123_DONE:
                size += bytes;
                this->eof = true;
            default:
                return size;
        }
    }

    return size;
}

bool MP3Decoder::Seek(double position)
{
    off_t offset = (off_t)(position * (double)this->sampleRate);

    if (offset < 0)
        return false;

    if (mpg123_seek(this->handle, offset, SEEK_SET) >= 0)
    {
        this->eof = false;
        return true;
    }
    else
        return false;
}

bool MP3Decoder::Rewind()
{
    this->eof = false;

    if (mpg123_seek(this->handle, 0, SEEK_SET) >= 0)
        return true;
    else
        return false;
}

bool MP3Decoder::IsSeekable()
{
    return true;
}

int MP3Decoder::GetChannelCount() const
{
    return this->channels;
}

int MP3Decoder::GetBitDepth() const
{
    return 16;
}

double MP3Decoder::GetDuration()
{
    if (this->duration == -2.0)
    {
        mpg123_scan(this->handle);

        off_t length = mpg123_length(this->handle);

        if (length == MPG123_ERR || length < 0)
            duration = -1.0;
        else
            duration = (double)length / (double)this->sampleRate;
    }

    return duration;
}
