#include <common/exception.hpp>

#define DRMP3_IMPLEMENTATION
#include <utilities/decoder/types/mp3decoder.hpp>

#include <algorithm>

using namespace love;

static bool isHeaderValid(const uint8_t* header)
{
    bool syncBits = header[0] == 0xFF && ((header[1] & 0xF0) || header[1] & 0xFE);

    bool layer      = DRMP3_HDR_GET_LAYER(header) != 0;
    bool bitRate    = DRMP3_HDR_GET_BITRATE(header) != 15;
    bool sampleRate = DRMP3_HDR_GET_SAMPLE_RATE(header) != 3;

    return syncBits && layer && bitRate && sampleRate;
}

static int64_t findFirstValidHeader(Stream* stream)
{
    constexpr size_t LOOKUP_SIZE = 16384;

    std::vector<uint8_t> data(LOOKUP_SIZE);

    uint8_t header[10];
    uint8_t* dataPtr = data.data();

    int64_t buffer = 0;
    int64_t offset = 0;

    if (stream->Read(header, 10) < 10)
        return -1;

    if (memcmp(header, "TAG", 3) == 0)
    {
        // ID3v1 tag is always 128 bytes long
        if (!stream->Seek(128, Stream::ORIGIN_BEGIN))
            return -1;

        buffer = stream->Read(dataPtr, LOOKUP_SIZE);
        offset = 128;
    }
    else if (memcmp(header, "ID3", 3) == 0)
    {
        // ID3v2 tag header is 10 bytes long, but we're
        // only interested on how much we should skip.
        int64_t position = header[9] | ((int64_t)header[8] << 7) | ((int64_t)header[7] << 14) |
                           ((int64_t)header[6] << 21);

        if (!stream->Seek(position, Stream::ORIGIN_CURRENT))
            return -1;

        buffer = stream->Read(dataPtr, LOOKUP_SIZE);
        offset = offset + 10;
    }
    else
    {
        // Copy the rest to data buffer
        std::copy_n(header, 10, dataPtr);
        buffer = 10 + stream->Read(dataPtr + 10, LOOKUP_SIZE - 10);
    }

    // Look for mp3 data
    for (int i = 0; i < buffer - 4; i++, offset++)
    {
        if (isHeaderValid(dataPtr++))
        {
            stream->Seek(offset, Stream::ORIGIN_BEGIN);

            return offset;
        }
    }

    // No valid MP3 frame found in first 16KB data
    return -1;
}

/* callbacks */

size_t MP3Decoder::OnRead(void* source, void* buffer, size_t count)
{
    auto decoder = (MP3Decoder*)source;
    int64_t read = decoder->stream->Read(buffer, count);

    return std::max<ssize_t>(0, read);
}

drmp3_bool32 MP3Decoder::OnSeek(void* source, int offset, drmp3_seek_origin origin)
{
    auto decoder  = (MP3Decoder*)source;
    auto position = decoder->offset;

    // Due to possible offsets, we have to calculate the position ourself.
    switch (origin)
    {
        case drmp3_seek_origin_start:
            position += offset;
            break;
        case drmp3_seek_origin_current:
            position = decoder->stream->Tell() + offset;
            break;
        default:
            return DRMP3_FALSE;
    }

    if (position < decoder->offset)
        return DRMP3_FALSE;

    return decoder->stream->Seek(position, Stream::ORIGIN_BEGIN) ? DRMP3_TRUE : DRMP3_FALSE;
}

/* end callbacks */

MP3Decoder::MP3Decoder(Stream* stream, int bufferSize) : Decoder(stream, bufferSize)
{
    // Check for possible ID3 tag and skip it if necessary.
    this->offset = findFirstValidHeader(stream);
    if (this->offset == -1)
        throw love::Exception("Could not find first valid mp3 header.");

    // initialize mp3 handle
    if (!drmp3_init(&this->handle, OnRead, OnSeek, this, nullptr))
        throw love::Exception("Could not read mp3 data.");

    this->sampleRate = this->handle.sampleRate;

    // calculate duration
    drmp3_uint64 pcmCount, mp3FrameCount;
    if (!drmp3_get_mp3_and_pcm_frame_count(&this->handle, &mp3FrameCount, &pcmCount))
    {
        drmp3_uninit(&this->handle);
        throw love::Exception("Could not calculate mp3 duration.");
    }
    this->duration = ((double)pcmCount) / ((double)this->handle.sampleRate);

    // create seek table
    drmp3_uint32 mp3FrameInt = (drmp3_uint32)mp3FrameCount;
    this->seekTable.resize((size_t)mp3FrameCount, { 0ULL, 0ULL, 0, 0 });

    if (!drmp3_calculate_seek_points(&this->handle, &mp3FrameInt, this->seekTable.data()))
    {
        drmp3_uninit(&this->handle);
        throw love::Exception("Could not calculate mp3 seek table.");
    }

    // bind seek table
    if (!drmp3_bind_seek_table(&this->handle, mp3FrameInt, this->seekTable.data()))
    {
        drmp3_uninit(&this->handle);
        throw love::Exception("Could not bind mp3 seek table");
    }
}

MP3Decoder::~MP3Decoder()
{
    drmp3_uninit(&this->handle);
}

Decoder* MP3Decoder::Clone()
{
    StrongReference<Stream> cloneStream(this->stream->Clone(), Acquire::NORETAIN);

    return new MP3Decoder(cloneStream, bufferSize);
}

int MP3Decoder::Decode()
{
    // bufferSize is in char
    const int maxRead = this->bufferSize / sizeof(int16_t) / this->handle.channels;
    int read =
        (int)drmp3_read_pcm_frames_s16(&this->handle, maxRead, (drmp3_int16*)this->GetBuffer());

    if (read < maxRead)
        this->eof = true;

    return read * sizeof(int16_t) * this->handle.channels;
}

bool MP3Decoder::Seek(double position)
{
    const drmp3_uint64 targetSample = (drmp3_uint64)(position * this->handle.sampleRate);
    drmp3_bool32 success            = drmp3_seek_to_pcm_frame(&this->handle, targetSample);

    if (success)
        this->eof = false;

    return success;
}

bool MP3Decoder::Rewind()
{
    return this->Seek(0);
}

bool MP3Decoder::IsSeekable()
{
    return true;
}

int MP3Decoder::GetChannelCount() const
{
    return this->handle.channels;
}

int MP3Decoder::GetBitDepth() const
{
    return 16;
}

double MP3Decoder::GetDuration()
{
    return this->duration;
}
