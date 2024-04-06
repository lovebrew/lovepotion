#include "modules/sound/lullaby/WaveDecoder.hpp"
#include "common/Exception.hpp"

#include <string.h>

namespace love
{
    static wuff_sint32 read_callback(void* userdata, wuff_uint8* buffer, size_t* size)
    {
        auto* stream = (Stream*)userdata;

        size_t readsize = stream->read(buffer, *size);
        *size           = readsize;

        return WUFF_SUCCESS;
    }

    static wuff_sint32 seek_callback(void* userdata, wuff_uint64 offset)
    {
        auto* stream = (Stream*)userdata;
        stream->seek(offset, Stream::SEEKORIGIN_BEGIN);

        return WUFF_SUCCESS;
    }

    static wuff_sint32 tell_callback(void* userdata, wuff_uint64* offset)
    {
        auto* stream = (Stream*)userdata;
        *offset      = stream->tell();

        return WUFF_SUCCESS;
    }

    static wuff_callback WaveDecoderCallbacks = { read_callback, seek_callback, tell_callback };

    WaveDecoder::WaveDecoder(Stream* stream, int bufferSize) : Decoder(stream, bufferSize)
    {
        int status = wuff_open(&this->handle, &WaveDecoderCallbacks, stream);

        if (status < 0)
            throw love::Exception("Could not open WAVE");

        try
        {
            status = wuff_stream_info(this->handle, &this->info);

            if (status < 0)
                throw love::Exception("Could not retrieve WAVE stream info");

            if (this->info.channels > 2)
                throw love::Exception("WAVE Multichannel audio not supported");

            if (this->info.format != WUFF_FORMAT_PCM_U8 && this->info.format != WUFF_FORMAT_PCM_S16)
            {
                status = wuff_format(this->handle, WUFF_FORMAT_PCM_S16);

                if (status < 0)
                    throw love::Exception("Could not convert WAVE to PCM_S16");
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

    Decoder* WaveDecoder::clone()
    {
        StrongRef<Stream> _stream(this->stream->clone(), Acquire::NO_RETAIN);
        return new WaveDecoder(_stream, this->bufferSize);
    }

    int WaveDecoder::decode()
    {
        size_t size = 0;

        while (size < (size_t)this->bufferSize)
        {
            size_t bytes = this->bufferSize - size;
            int status   = wuff_read(this->handle, (wuff_uint8*)this->buffer + size, &bytes);

            if (status < 0)
                return 0;
            else if (bytes == 0)
            {
                this->eof = true;
                break;
            }

            size += bytes;
        }

        return (int)size;
    }

    bool WaveDecoder::seek(double position)
    {
        int status = wuff_seek(this->handle, (wuff_uint64)(position * this->info.sample_rate));

        if (status >= 0)
        {
            this->eof = false;
            return true;
        }

        return false;
    }

    bool WaveDecoder::rewind()
    {
        int status = wuff_seek(this->handle, 0);

        if (status >= 0)
        {
            this->eof = false;
            return true;
        }

        return false;
    }

    bool WaveDecoder::isSeekable()
    {
        return true;
    }

    int WaveDecoder::getChannelCount() const
    {
        return this->info.channels;
    }

    int WaveDecoder::getBitDepth() const
    {
        return this->info.bits_per_sample == 8 ? 8 : 16;
    }

    int WaveDecoder::getSampleRate() const
    {
        return this->info.sample_rate;
    }

    double WaveDecoder::getDuration()
    {
        return (double)this->info.length / this->info.sample_rate;
    }
} // namespace love
