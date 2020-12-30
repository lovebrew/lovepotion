
#include "flacdecoder.h"

using namespace love;

static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder * decoder, const FLAC__Frame * frame, const FLAC__int32 * const buffer[], void * client_data)
{
    FLACDecoder::FLACFile * file = (FLACDecoder::FLACFile *)client_data;

    if (file->channels != (int)frame->header.channels || file->sampleRate != (int)frame->header.sample_rate)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    for (unsigned sample = 0; sample < frame->header.blocksize; sample++)
    {
        for (unsigned channel = 0; channel < frame->header.channels; channel++)
        {
            *(file->writePointer++) = buffer[channel][sample];
            file->usedSamples += 1;
        }
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void error_callback(const FLAC__StreamDecoder * decoder, FLAC__StreamDecoderErrorStatus status, void * client_data)
{
    // #if defined (__DEBUG__)
    //     const char * str = nullptr;
    //     LOG("FLAC: decoder error callback was called: %d\n", status);
    //     if ((str = FLAC__StreamDecoderErrorStatusString[status]))
    //         LOG("FLAC: status of %d: %s\n", status, str);
    // #endif
}

static void metadata_callback(const FLAC__StreamDecoder * decoder, const FLAC__StreamMetadata * metadata, void * client_data)
{
    FLACDecoder::FLACFile * file = (FLACDecoder::FLACFile *)client_data;

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        file->PCMCount = metadata->data.stream_info.total_samples;
        file->bitsPerSample = metadata->data.stream_info.bits_per_sample;
        file->channels = metadata->data.stream_info.channels;
        file->sampleRate = metadata->data.stream_info.sample_rate;
    }
}

 static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder * decoder, FLAC__byte buffer [], size_t * bytes, void * client_data)
 {
   FLACDecoder::FLACFile * file = ((FLACDecoder::FLACFile *)client_data);

   if (*bytes > 0)
   {
        if (*bytes == 0)
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        else
        {
            memcpy(buffer, (const char * )file->data, *bytes);
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
   }
   else
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
 }

bool FLACDecoder::inited = false;

FLACDecoder::FLACDecoder(Data * data, int bufferSize) : Decoder(data, bufferSize)
{
    this->decoder = FLAC__stream_decoder_new();

    if (!this->decoder)
        throw love::Exception("Failed to create FLAC decoder!");

    this->file.data = (const char *)data->GetData();
    this->file.size = data->GetSize();
    this->file.read = 0;

    this->status = FLAC__stream_decoder_init_stream(this->decoder, read_callback, NULL, NULL,
                                                    NULL, NULL, write_callback,
                                                    metadata_callback, error_callback,
                                                    &this->file);

    if (this->status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        throw love::Exception("Could not initialize the main FLAC decoder: %s (%d)\n", FLAC__StreamDecoderInitStatusString[this->status], this->status);

    FLAC__bool success = FLAC__stream_decoder_process_until_end_of_metadata(this->decoder);

    if (!success)
        throw love::Exception("Failed to read FLAC metadata!");

    FLACDecoder::inited = true;
}

FLACDecoder::~FLACDecoder()
{
    FLAC__stream_decoder_finish(this->decoder);
    FLAC__stream_decoder_delete(this->decoder);

    FLACDecoder::inited = false;
}

bool FLACDecoder::Accepts(const std::string & ext)
{
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


#define BUFTYPE_BITS 16

static void convertBuffersGeneric(int32_t * src, int16_t * dst, unsigned count, unsigned res)
{
    int32_t * rp  = src;
    int16_t * wp2 = dst;

    if (res < BUFTYPE_BITS)
    {
        const uint8_t kPower = (uint8_t)(BUFTYPE_BITS - res);

        for (unsigned i = 0; i < count; i++)
            *wp2++ = static_cast<int16_t>(rp[i]) << kPower;
    }
    else if (res > BUFTYPE_BITS)
    {
        const uint8_t kPower = (uint8_t)(res - BUFTYPE_BITS);

        for (unsigned i = 0; i < count; i++)
            *wp2++ = static_cast<int16_t>(rp[i] >> kPower);
    }
    else
    {
        for (unsigned i = 0; i < count; i++)
            *wp2++ = static_cast<int16_t>(rp[i]);
    }
}

int FLACDecoder::Decode(s16 * buffer)
{
    if (FLAC__stream_decoder_get_state(this->decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
        return 0;

    this->file.usedSamples = 0;

    this->file.buffer = new int32_t[this->bufferSize];
    memset(this->file.buffer, 0, sizeof(int32_t) * this->bufferSize);
    this->file.writePointer = &this->file.buffer[0];

    /* Try to decode a single frame of audio */
    if (FLAC__stream_decoder_process_single(this->decoder) == false)
        return 0;

    convertBuffersGeneric(this->file.buffer, buffer, this->file.usedSamples, this->file.PCMCount);

    return this->file.usedSamples;
}

bool FLACDecoder::Seek(double position)
{
    if (position > this->file.PCMCount)
        return false;

    return FLAC__stream_decoder_seek_absolute(this->decoder, position);
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