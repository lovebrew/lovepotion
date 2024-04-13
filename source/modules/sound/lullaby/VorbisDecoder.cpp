#include "modules/sound/lullaby/VorbisDecoder.hpp"
#include "common/Exception.hpp"

#include <string.h>

namespace love
{
    static int vorbisClose(void*)
    {
        // Does nothing (handled elsewhere)
        return 1;
    }

    static size_t vorbisRead(void* pointer, size_t byteSize, size_t sizeToRead, void* source)
    {
        auto stream = (Stream*)source;

        return stream->read(pointer, byteSize * sizeToRead);
    }

    static int vorbisSeek(void* source, ogg_int64_t offset, int whence)
    {
        auto stream = (Stream*)source;
        auto origin = Stream::SEEKORIGIN_BEGIN;

        switch (whence)
        {
            case SEEK_SET:
                origin = Stream::SEEKORIGIN_BEGIN;
                break;
            case SEEK_CUR:
                origin = Stream::SEEKORIGIN_CURRENT;
                break;
            case SEEK_END:
                origin = Stream::SEEKORIGIN_END;
                break;
            default:
                break;
        };

        return stream->seek(offset, origin) ? 0 : -1;
    }

    static long vorbisTell(void* source)
    {
        auto stream = (Stream*)source;

        return (long)stream->tell();
    }

    VorbisDecoder::VorbisDecoder(Stream* stream, int bufferSize) :
        Decoder(stream, bufferSize),
        duration(-2.0)
    {
        ov_callbacks callbacks {};
        callbacks.close_func = vorbisClose;
        callbacks.seek_func  = vorbisSeek;
        callbacks.read_func  = vorbisRead;
        callbacks.tell_func  = vorbisTell;

        if (ov_open_callbacks(stream, &this->handle, nullptr, 0, callbacks) < 0)
            throw love::Exception("Could not open Ogg bitstream");

        this->vorbisInfo = ov_info(&this->handle, -1);
    }

    VorbisDecoder::~VorbisDecoder()
    {
        ov_clear(&this->handle);
    }

    Decoder* VorbisDecoder::clone()
    {
        StrongRef<Stream> _stream(this->stream->clone(), Acquire::NO_RETAIN);
        return new VorbisDecoder(_stream, bufferSize);
    }

    int VorbisDecoder::decode()
    {
        int size    = 0;
        int section = 0;

        long read = 0;

#if defined(__WIIU__)
        int word = (this->getBitDepth() == 16 ? 2 : 1);
#endif

        while (size < this->bufferSize)
        {
            int length = this->bufferSize - size;

#if !defined(__WIIU__)
            read = ov_read(&this->handle, (char*)this->buffer + size, length, &section);
#else
            read = ov_read(&this->handle, (char*)this->buffer + size, length, 1, word, 1, &section);
#endif

            if (read == OV_HOLE)
                continue;
            else if (read <= OV_EREAD)
                return -1;
            else if (read == 0)
            {
                this->eof = true;
                break;
            }
            else if (read > 0)
                size += read;
        }

        return size;
    }

    bool VorbisDecoder::seek(double position)
    {
        int result = 0;

        if (position <= VorbisDecoder::EPSILON)
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

    bool VorbisDecoder::rewind()
    {
        int result = ov_raw_seek(&this->handle, 0);

        if (result == 0)
        {
            this->eof = false;
            return true;
        }

        return false;
    }

    bool VorbisDecoder::isSeekable()
    {
        long result = ov_seekable(&this->handle);

        return (result != 0);
    }

    int VorbisDecoder::getChannelCount() const
    {
        return this->vorbisInfo->channels;
    }

    int VorbisDecoder::getBitDepth() const
    {
        return 16;
    }

    int VorbisDecoder::getSampleRate() const
    {
        return this->vorbisInfo->rate;
    }

    double VorbisDecoder::getDuration()
    {
        if (this->duration == -2.0)
        {
            this->duration = ov_time_total(&this->handle, -1) / 1000.0;

            if (this->duration == OV_EINVAL || this->duration < 0.0)
                this->duration = -1.0;
        }

        return duration;
    }
} // namespace love
