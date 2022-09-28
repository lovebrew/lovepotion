#include <common/exception.hpp>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_OGG
#include <utilities/decoder/types/flacdecoder.hpp>
#include <utilities/log/logfile.h>

using namespace love;

/* callbacks */

static size_t read(void* source, void* data, size_t bytesToRead)
{
    auto* stream = (Stream*)source;
    int64_t read = stream->Read(data, bytesToRead);

    return std::max<int64_t>(0, read);
}

static drflac_bool32 seek(void* source, int offset, drflac_seek_origin whence)
{
    auto* stream = (Stream*)source;
    auto origin  = (whence == drflac_seek_origin_current) ? Stream::SeekOrigin::ORIGIN_CURRENT
                                                          : Stream::SeekOrigin::ORIGIN_BEGIN;

    return stream->Seek(offset, origin) ? DRFLAC_TRUE : DRFLAC_FALSE;
}

/* end callbacks */

FLACDecoder::FLACDecoder(Stream* stream, int bufferSize) :
    Decoder(stream, bufferSize),
    handle(nullptr)
{
    this->handle = drflac_open(read, seek, stream, nullptr);

    if (!this->handle)
        throw love::Exception("Could not load FLAC file!");
}

FLACDecoder::~FLACDecoder()
{
    drflac_close(this->handle);
}

Decoder* FLACDecoder::Clone()
{
    StrongReference<Stream> cloneStream(this->stream->Clone(), Acquire::NORETAIN);

    return new FLACDecoder(cloneStream, bufferSize);
}

int FLACDecoder::Decode()
{
    drflac_uint64 read =
        drflac_read_pcm_frames_s16(this->handle, this->bufferSize / 2 / this->handle->channels,
                                   (drflac_int16*)this->buffer.get());
    read *= 2 * this->handle->channels;

    if ((int)read < this->bufferSize)
        this->eof = true;

    return (int)read;
}

bool FLACDecoder::Seek(double position)
{
    const auto seekTo    = (drflac_uint64)(position * this->handle->sampleRate);
    drflac_bool32 result = drflac_seek_to_pcm_frame(this->handle, seekTo);

    if (result)
        this->eof = false;

    return result;
}

bool FLACDecoder::Rewind()
{
    return this->Seek(0);
}

bool FLACDecoder::IsSeekable()
{
    return true;
}

int FLACDecoder::GetChannelCount() const
{
    return this->handle->channels;
}

int FLACDecoder::GetBitDepth() const
{
    return 16;
}

int FLACDecoder::GetSampleRate() const
{
    return this->handle->sampleRate;
}

double FLACDecoder::GetDuration()
{
    return ((double)this->handle->totalPCMFrameCount / (double)this->handle->sampleRate);
}
