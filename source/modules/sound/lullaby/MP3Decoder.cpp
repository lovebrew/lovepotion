#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include "modules/sound/lullaby/MP3Decoder.hpp"
#include "common/Exception.hpp"

namespace love
{
    static int64_t findFirstValidHeader(Stream* stream)
    {
        // Tweaking this variable has trade-off between false-positive and false-negative. Lesser
        // value
        // means lesser false-positive and false-negative. Larger value means more false-positive
        // AND false-negative.
        constexpr size_t LOOKUP_SIZE = 128;

        std::vector<uint8_t> data(LOOKUP_SIZE);
        uint8_t header[10];
        uint8_t* dataPtr = data.data();
        int64_t buffer   = 0;
        int64_t offset   = 0;

        if (stream->read(header, 10) < 10)
            return -1;

        // Test for known audio formats which are definitely not MP3.
        if (memcmp(header, "RIFF", 4) == 0)
            return -1;
        if (memcmp(header, "OggS", 4) == 0)
            return -1;
        if (memcmp(header, "fLaC", 4) == 0)
            return -1;

        if (memcmp(header, "TAG", 3) == 0)
        {
            // ID3v1 tag is always 128 bytes long
            if (!stream->seek(128, Stream::SEEKORIGIN_BEGIN))
                return -1;

            buffer = stream->read(dataPtr, LOOKUP_SIZE);
            offset = 128;
        }
        else if (memcmp(header, "ID3", 3) == 0)
        {
            // ID3v2 tag header is 10 bytes long, but we're
            // only interested on how much we should skip.
            int64_t off = header[9] | ((int64_t)header[8] << 7) | ((int64_t)header[7] << 14) |
                          ((int64_t)header[6] << 21);

            if (!stream->seek(off, Stream::SEEKORIGIN_CURRENT))
                return -1;

            buffer = stream->read(dataPtr, LOOKUP_SIZE);
            offset = off + 10;
        }
        else
        {
            // Copy the rest to data buffer
            memcpy(dataPtr, header, 10);
            buffer = 10 + stream->read(dataPtr + 10, LOOKUP_SIZE - 10);
        }

        // Look for mp3 data
        for (int i = 0; i < buffer - 4; i++, offset++)
        {
            if (drmp3_hdr_valid(dataPtr++))
            {
                stream->seek(offset, Stream::SEEKORIGIN_BEGIN);
                return offset;
            }
        }

        // No valid MP3 frame found in first few bytes of the data.
        return -1;
    }

    size_t MP3Decoder::onRead(void* userData, void* bufferOut, size_t bytesToRead)
    {
        auto* decoder     = (MP3Decoder*)userData;
        int64_t bytesRead = decoder->stream->read(bufferOut, bytesToRead);

        return std::max<int64_t>(0, bytesRead);
    }

    drmp3_bool32 MP3Decoder::onSeek(void* userData, int offset, drmp3_seek_origin origin)
    {
        auto* decoder    = (MP3Decoder*)userData;
        int64_t position = decoder->offset;

        switch (origin)
        {
            case drmp3_seek_origin_start:
                position += offset;
                break;
            case drmp3_seek_origin_current:
                position = decoder->stream->tell() + offset;
                break;
            default:
                return DRMP3_FALSE;
        }

        if (position < decoder->offset)
            return DRMP3_FALSE;

        return decoder->stream->seek(offset, Stream::SEEKORIGIN_BEGIN) ? DRMP3_TRUE : DRMP3_FALSE;
    }

    MP3Decoder::MP3Decoder(Stream* stream, int bufferSize) : Decoder(stream, bufferSize)
    {
        if (findFirstValidHeader(stream) == -1)
            throw love::Exception("Could not find first valid MP3 header.");

        if (!drmp3_init(&this->handle, onRead, onSeek, this, nullptr))
            throw love::Exception("Could not read MP3 data.");

        this->sampleRate = this->handle.sampleRate;
        drmp3_uint64 pcmCount, mp3FrameCount;

        if (!drmp3_get_mp3_and_pcm_frame_count(&this->handle, &mp3FrameCount, &pcmCount))
        {
            drmp3_uninit(&this->handle);
            throw love::Exception("Could not calculate mp3 duration.");
        }
        this->duration = ((double)pcmCount) / this->handle.sampleRate;

        auto mp3FrameInt = (drmp3_uint32)mp3FrameCount;
        this->seekTable.resize((size_t)mp3FrameCount, { 0ULL, 0ULL, 0, 0 });

        if (!drmp3_calculate_seek_points(&this->handle, &mp3FrameInt, this->seekTable.data()))
        {
            drmp3_uninit(&this->handle);
            throw love::Exception("Could not calculate MP3 seek table.");
        }

        if (!drmp3_bind_seek_table(&this->handle, mp3FrameInt, this->seekTable.data()))
        {
            drmp3_uninit(&this->handle);
            throw love::Exception("Could not bind MP3 seek table.");
        }
    }

    MP3Decoder::~MP3Decoder()
    {
        drmp3_uninit(&this->handle);
    }

    Decoder* MP3Decoder::clone()
    {
        StrongRef<Stream> _stream(this->stream->clone(), Acquire::NO_RETAIN);
        return new MP3Decoder(_stream, bufferSize);
    }

    int MP3Decoder::decode()
    {
        auto maxRead = this->bufferSize / sizeof(int16_t) / this->handle.channels;
        int read = drmp3_read_pcm_frames_s16(&this->handle, maxRead, (drmp3_int16*)this->buffer);

        if (read < (int)maxRead)
            this->eof = true;

        return read * sizeof(int16_t) * this->handle.channels;
    }

    bool MP3Decoder::seek(double position)
    {
        auto targetSample = (drmp3_uint64)(position * this->handle.sampleRate);
        auto success      = drmp3_seek_to_pcm_frame(&this->handle, targetSample);

        if (success)
            this->eof = true;

        return success;
    }

    bool MP3Decoder::rewind()
    {
        return this->seek(0.0);
    }

    bool MP3Decoder::isSeekable()
    {
        return true;
    }

    int MP3Decoder::getChannelCount() const
    {
        return this->handle.channels;
    }

    int MP3Decoder::getBitDepth() const
    {
        return 16;
    }

    double MP3Decoder::getDuration()
    {
        return this->duration;
    }

} // namespace love
