
#include "flacdecoder.h"

using namespace love;

/* this is stupid and should be fixed */
#define BUFFER_SIZE_SAMP (FLAC__MAX_BLOCK_SIZE * FLAC__MAX_CHANNELS)

static void convertBuffers(int32_t* source, int16_t* destination, size_t count, size_t resolution)
{
    int32_t* readBuffer  = source;
    int16_t* writeBuffer = destination;

    if (resolution < Decoder::DEFAULT_BIT_DEPTH)
    {
        const uint8_t kPower = static_cast<uint8_t>(Decoder::DEFAULT_BIT_DEPTH - resolution);

        for (unsigned i = 0; i < count; i++)
            *writeBuffer++ = readBuffer[i] << kPower;
    }
    else if (resolution > Decoder::DEFAULT_BIT_DEPTH)
    {
        const uint8_t kPower = static_cast<uint8_t>(resolution - Decoder::DEFAULT_BIT_DEPTH);

        for (unsigned i = 0; i < count; i++)
            *writeBuffer++ = readBuffer[i] >> kPower;
    }
    else
    {
        for (unsigned i = 0; i < count; i++)
            *writeBuffer++ = readBuffer[i];
    }
}

static void callbackFrameClear(FLACDecoder::FLACFile* file)
{
    file->bufferUsed = 0;
    memset(file->outputBuffer, 0, BUFFER_SIZE_SAMP);
    file->writeBuffer = &file->outputBuffer[0];
}

static FLAC__StreamDecoderReadStatus readCallback(const FLAC__StreamDecoder* decoder,
                                                  FLAC__byte buffer[], size_t* bytes,
                                                  void* clientData)
{
    size_t spaceUntilEOF  = 0;
    size_t actualSizeRead = 0;

    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;
    spaceUntilEOF               = file->size - file->read;

    if (*bytes < spaceUntilEOF)
        actualSizeRead = *bytes;
    else
        actualSizeRead = spaceUntilEOF;

    if (actualSizeRead)
    {
        memcpy(buffer, file->data + file->read, actualSizeRead);
        file->read += actualSizeRead;
    }
    else
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

    *bytes = actualSizeRead;

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

static FLAC__StreamDecoderWriteStatus writeCallback(const FLAC__StreamDecoder* decoder,
                                                    const FLAC__Frame* frame,
                                                    const FLAC__int32* const buffer[],
                                                    void* clientData)
{
    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;
    callbackFrameClear(file);

    if (file->channels != frame->header.channels || file->sampleRate != frame->header.sample_rate)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    for (size_t sample = 0; sample < frame->header.blocksize; sample++)
    {
        for (size_t channel = 0; channel < frame->header.channels; channel++)
        {
            *(file->writeBuffer++) = buffer[channel][sample];
            file->bufferUsed += 1;
        }
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void errorCallback(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status,
                          void* clientData)
{}

void metadataCallback(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata,
                      void* clientData)
{
    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        file->totalSamples  = metadata->data.stream_info.total_samples;
        file->bitsPerSample = metadata->data.stream_info.bits_per_sample;

        file->channels   = metadata->data.stream_info.channels;
        file->sampleRate = metadata->data.stream_info.sample_rate;
    }
}

static FLAC__StreamDecoderSeekStatus seekCallback(const FLAC__StreamDecoder* decoder,
                                                  FLAC__uint64 offset, void* clientData)
{
    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;

    if (file->read + offset > file->size)
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;

    file->read = offset;

    return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

static FLAC__StreamDecoderTellStatus tellCallback(const FLAC__StreamDecoder* decoder,
                                                  FLAC__uint64* offset, void* clientData)
{
    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;

    *offset = file->read;

    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderLengthStatus lengthCallback(const FLAC__StreamDecoder* decoder,
                                                      FLAC__uint64* length, void* clientData)
{
    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;

    *length = file->size;

    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static FLAC__bool eofCallback(const FLAC__StreamDecoder* decoder, void* clientData)
{
    FLACDecoder::FLACFile* file = (FLACDecoder::FLACFile*)clientData;

    return file->size <= file->read;
}

FLACDecoder::FLACDecoder(Data* data, int bufferSize) : Decoder(data, bufferSize)
{
    this->decoder = FLAC__stream_decoder_new();

    if (!this->decoder)
        throw love::Exception("Failed to create FLAC decoder!");

    this->file = FLACDecoder::FLACFile();

    this->file.data = (const char*)data->GetData();
    this->file.size = data->GetSize();

    this->file.outputBuffer = new int32_t[BUFFER_SIZE_SAMP];
    this->file.read         = 0;

    this->status = FLAC__stream_decoder_init_stream(
        this->decoder, readCallback, seekCallback, tellCallback, lengthCallback, eofCallback,
        writeCallback, metadataCallback, errorCallback, &this->file);

    if (this->status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        throw love::Exception("Could not initialize the main FLAC decoder: %s (%d)\n",
                              FLAC__StreamDecoderInitStatusString[this->status], this->status);

    FLAC__bool success = FLAC__stream_decoder_process_until_end_of_metadata(this->decoder);

    if (!success)
        throw love::Exception("Failed to read FLAC metadata!");
}

FLACDecoder::~FLACDecoder()
{
    FLAC__stream_decoder_finish(this->decoder);
    FLAC__stream_decoder_delete(this->decoder);

    delete this->file.outputBuffer;
}

bool FLACDecoder::Accepts(const std::string& ext)
{
    static const std::string supported[] = { "flac", "ogg", "" };

    for (int i = 0; !(supported[i].empty()); i++)
    {
        if (supported[i].compare(ext) == 0)
            return true;
    }

    return false;
}

Decoder* FLACDecoder::Clone()
{
    return new FLACDecoder(this->data.Get(), this->bufferSize);
}

int FLACDecoder::Decode()
{
    return this->Decode((s16*)this->buffer);
}

int FLACDecoder::Decode(s16* buffer)
{
    size_t read = 0;

    if (FLAC__stream_decoder_get_state(this->decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
        return 0;

    if (this->decodeBufferRead >= this->file.bufferUsed * sizeof(int16_t) || !this->file.bufferUsed)
    {
        if (!FLAC__stream_decoder_process_single(this->decoder))
            return 0;

        this->decodeBufferRead = 0;
    }

    if (this->file.bufferUsed)
    {
        if (this->decodeBufferRead + this->bufferSize > this->file.bufferUsed * sizeof(int16_t))
            read = this->bufferSize -
                   (this->file.bufferUsed * sizeof(int16_t) - this->decodeBufferRead);
        else
            read = this->bufferSize;

        convertBuffers(this->file.outputBuffer + this->decodeBufferRead, buffer,
                       read / sizeof(int16_t), this->GetBitDepth());

        this->decodeBufferRead += read;
    }

    return read;
}

bool FLACDecoder::Seek(double position)
{
    if (position > this->file.totalSamples)
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
    return ((double)this->file.totalSamples) / ((double)this->file.sampleRate);
}
