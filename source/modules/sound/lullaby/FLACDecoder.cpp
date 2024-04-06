#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_OGG
#include "modules/sound/lullaby/FLACDecoder.hpp"
#include "common/Exception.hpp"

#include <algorithm>
#include <set>

namespace love
{
    static size_t onRead(void* userData, void* bufferOut, size_t bytesToRead)
    {
        Stream* stream = (Stream*)userData;
        int64_t read   = stream->read(bufferOut, bytesToRead);

        return std::max<int64_t>(0, read);
    }

    static drflac_bool32 onSeek(void* userData, int offset, drflac_seek_origin origin)
    {
        Stream* stream  = (Stream*)userData;
        auto seekOrigin = origin == drflac_seek_origin_current ? Stream::SEEKORIGIN_CURRENT
                                                               : Stream::SEEKORIGIN_BEGIN;

        return stream->seek(offset, seekOrigin) ? DRFLAC_TRUE : DRFLAC_FALSE;
    }

    FLACDecoder::FLACDecoder(Stream* stream, int bufferSize) : Decoder(stream, bufferSize)
    {
        this->handle = drflac_open(onRead, onSeek, stream, nullptr);

        if (this->handle == nullptr)
            throw love::Exception("Could not load FLAC file");
    }

    FLACDecoder::~FLACDecoder()
    {
        drflac_close(this->handle);
    }

    Decoder* FLACDecoder::clone()
    {
        StrongRef<Stream> _stream(this->stream->clone(), Acquire::NO_RETAIN);
        return new FLACDecoder(_stream, this->bufferSize);
    }

    int FLACDecoder::decode()
    {
        const auto size = this->bufferSize / 2 / this->handle->channels;

        auto read = drflac_read_pcm_frames_s16(this->handle, size, (drflac_int16*)this->buffer);
        read *= 2 * this->handle->channels;

        if ((int)read < this->bufferSize)
            this->eof = true;

        return (int)read;
    }

    bool FLACDecoder::seek(double position)
    {
        auto seekPosition = (drflac_uint64)(position * this->handle->sampleRate);
        const auto result = drflac_seek_to_pcm_frame(this->handle, seekPosition);

        if (result)
            this->eof = false;

        return result;
    }

    bool FLACDecoder::rewind()
    {
        return this->seek(0.0);
    }

    bool FLACDecoder::isSeekable()
    {
        return true;
    }

    int FLACDecoder::getChannelCount() const
    {
        return this->handle->channels;
    }

    int FLACDecoder::getBitDepth() const
    {
        return 16;
    }

    int FLACDecoder::getSampleRate() const
    {
        return this->handle->sampleRate;
    }

    double FLACDecoder::getDuration()
    {
        return (double)(this->handle->totalPCMFrameCount / this->handle->sampleRate);
    }
} // namespace love
