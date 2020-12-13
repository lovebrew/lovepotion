
#include "common/runtime.h"

#define DR_FLAC_IMPLEMENTATION
#include "flacdecoder.h"

using namespace love;

static void metadata_callback(const FLAC__StreamDecoder * decoder, const FLAC__StreamMetadata * metadata, void * data)
{

}

static FLAC__StreamDecoderReadCallback read_callback(const FLAC__StreamDecoder * decoder, FLAC__byte * buffer, size_t * bytes, void * data)
{

}

static FLAC__StreamDecoderSeekCallback seek_callback()
{

}

static void tell_callback()
{}

FLACDecoder::FLACDecoder(Data * data, int bufferSize) : Decoder(data, bufferSize)
{
    // this->decoder = FLAC__stream_decoder_new();
    // if (!this->decoder)
    //     throw love::Exception("Failed to create FLAC decoder!");

    // FLAC__stream_decoder_set_md5_checking(this->decoder, true);

    // this->status = FLAC__stream_decoder_init_stream(this->decoder, read_callback, seek_callback, tell_callback, NULL, NULL, NULL, metadata_callback, NULL, &this->file);
}

FLACDecoder::~FLACDecoder()
{

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
    return 0;
}

bool FLACDecoder::Seek(double position)
{
    return false;
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
    return this->file.channels;
}

int FLACDecoder::GetBitDepth() const
{
    return 16;
}

int FLACDecoder::GetSampleRate() const
{
    return this->file.sampleRate;
}

double FLACDecoder::GetDuration()
{
    return ((double)this->file.PCMCount) / ((double)this->file.sampleRate);
}