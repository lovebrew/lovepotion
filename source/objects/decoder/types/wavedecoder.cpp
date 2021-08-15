#if defined(__3DS__)
    #include <3ds/types.h>
#elif defined(__SWITCH__)
    #include <switch/types.h>
#endif

#include "wavedecoder.h"

using namespace love;

static wuff_sint32 read_callback(void* source, wuff_uint8* buffer, size_t* size)
{
    WaveDecoder::WaveFile* file = (WaveDecoder::WaveFile*)source;

    size_t bytesLeft  = file->size - file->offset;
    size_t targetSize = (*size < bytesLeft) ? *size : bytesLeft;

    memcpy(buffer, file->data + file->offset, targetSize);
    file->offset += targetSize;

    *size = targetSize;

    return WUFF_SUCCESS;
}

static wuff_sint32 seek_callback(void* source, wuff_uint64 offset)
{
    WaveDecoder::WaveFile* file = (WaveDecoder::WaveFile*)source;

    file->offset = (size_t)((offset < file->size) ? offset : file->size);

    return WUFF_SUCCESS;
}

static wuff_sint32 tell_callback(void* source, wuff_uint64* offset)
{
    WaveDecoder::WaveFile* file = (WaveDecoder::WaveFile*)source;

    *offset = file->offset;

    return WUFF_SUCCESS;
}

wuff_callback callbacks = { read_callback, seek_callback, tell_callback };

WaveDecoder::WaveDecoder(Data* data, int bufferSize) : Decoder(data, bufferSize)
{
    this->file.data   = (char*)data->GetData();
    this->file.size   = data->GetSize();
    this->file.offset = 0;

    int status = wuff_open(&this->handle, &callbacks, &this->file);

    if (status < 0)
        throw love::Exception("Could not open WAVE.");

    try
    {
        status = wuff_stream_info(this->handle, &this->info);

        if (status < 0)
            throw love::Exception("Could not retrieve WAVE stream info.");

        if (this->info.channels > 2)
            throw love::Exception("Multichannel WAVE audio not supported.");

        if (info.format != WUFF_FORMAT_PCM_U8 && info.format != WUFF_FORMAT_PCM_S16)
        {
            status = wuff_format(this->handle, WUFF_FORMAT_PCM_S16);

            if (status < 0)
                throw love::Exception("Could not set WAVE output format.");
        }
    }
    catch (love::Exception& e)
    {
        wuff_close(this->handle);
        throw;
    }
}

WaveDecoder::~WaveDecoder()
{
    wuff_close(this->handle);
}

bool WaveDecoder::Accepts(const std::string& ext)
{
    static const std::string supported[] = { "wav", "" };

    for (int i = 0; !(supported[i].empty()); i++)
    {
        if (supported[i].compare(ext) == 0)
            return true;
    }

    return false;
}

Decoder* WaveDecoder::Clone()
{
    return new WaveDecoder(this->data.Get(), bufferSize);
}

int WaveDecoder::Decode()
{
    return this->Decode((s16*)this->buffer);
}

int WaveDecoder::Decode(s16* buffer)
{
    size_t size = 0;

    while (size < (size_t)this->bufferSize)
    {
        size_t bytes = this->bufferSize - size;

        int status = wuff_read(this->handle, (wuff_uint8*)buffer + size, &bytes);

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

bool WaveDecoder::Seek(double position)
{
    int status = wuff_seek(this->handle, (wuff_uint64)(position * this->info.sample_rate));

    if (status >= 0)
    {
        this->eof = false;
        return true;
    }

    return false;
}

bool WaveDecoder::Rewind()
{
    int status = wuff_seek(this->handle, 0);

    if (status >= 0)
    {
        this->eof = false;
        return true;
    }

    return false;
}

bool WaveDecoder::IsSeekable()
{
    return true;
}

int WaveDecoder::GetChannelCount() const
{
    return this->info.channels;
}

int WaveDecoder::GetBitDepth() const
{
    return (this->info.bits_per_sample == 8) ? 8 : 16;
}

int WaveDecoder::GetSampleRate() const
{
    return this->info.sample_rate;
}

double WaveDecoder::GetDuration()
{
    return (double)this->info.length / (double)this->info.sample_rate;
}
