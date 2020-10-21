
#include "common/runtime.h"

#define DR_FLAC_IMPLEMENTATION
#include "flacdecoder.h"

using namespace love;

FLACDecoder::FLACDecoder(Data * data, int bufferSize) : Decoder(data, bufferSize)
{
    this->flac = drflac_open_memory(data->GetData(), data->GetSize(), nullptr);

    if (this->flac == nullptr)
        throw love::Exception("Could not load FLAC file.");
}

FLACDecoder::~FLACDecoder()
{
    drflac_close(this->flac);
}

bool FLACDecoder::Accepts(const std::string & ext)
{
    // dr_flac supports FLAC encapsulated in Ogg, but unfortunately
    // LOVE detects .ogg extension as Vorbis. It would be a good idea
    // to always probe in the future (see #1487 and commit ccf9e63).
    // Please remove once it's no longer the case.
    static const std::string supported[] =
    {
        "flac", "ogg", ""
    };

    for (int i = 0; !(supported[i].empty()); i++)
    {
        if (supported[i].compare(ext) == 0)
            return true;
    }

    return false;
}

Decoder * FLACDecoder::Clone()
{
    return new FLACDecoder(this->data.Get(), this->bufferSize);
}

int FLACDecoder::Decode()
{
    return this->Decode((s16 *)this->buffer);
}

int FLACDecoder::Decode(s16 * buffer)
{
    // `bufferSize` is in bytes, so divide by 2.
    drflac_uint64 read = drflac_read_pcm_frames_s16(this->flac, this->bufferSize / 2 / this->flac->channels, (drflac_int16 *)buffer);
    read *= 2 * this->flac->channels;

    if ((int)read < this->bufferSize)
        eof = true;

    return (int)read;
}

bool FLACDecoder::Seek(double position)
{
    drflac_uint64 seekPosition = (drflac_uint64)(position * flac->sampleRate);

    drflac_bool32 result = drflac_seek_to_pcm_frame(flac, seekPosition);

    if (result)
        this->eof = false;

    return result;
}

bool FLACDecoder::Rewind()
{
    return Seek(0.0);
}

bool FLACDecoder::IsSeekable()
{
    return true;
}

int FLACDecoder::GetChannelCount() const
{
    return this->flac->channels;
}

int FLACDecoder::GetBitDepth() const
{
    return 16;
}

int FLACDecoder::GetSampleRate() const
{
    return this->flac->sampleRate;
}

double FLACDecoder::GetDuration()
{
    return ((double)this->flac->totalPCMFrameCount) / ((double)this->flac->sampleRate);
}