#include "common/runtime.h"

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include "mpegdecoder.h"

using namespace love;

MPEGDecoder::MPEGDecoder(Data * data, int bufferSize) : Decoder(data, bufferSize)
{
    if (drmp3_init_memory(&this->mp3, data->GetData(), data->GetSize(), nullptr, nullptr) == 0)
        throw love::Exception("Could not read mp3 data.");

    this->sampleRate = this->mp3.sampleRate;

    drmp3_uint64 pcmCount, mp3FrameCount;
    if (!drmp3_get_mp3_and_pcm_frame_count(&this->mp3, &mp3FrameCount, &pcmCount))
    {
        drmp3_uninit(&this->mp3);
        throw love::Exception("Could not calculate duration.");
    }
    this->duration = ((double)pcmCount / (double)this->mp3.sampleRate);

    uint32_t mp3FrameInt = mp3FrameCount;
    seekTable.resize(mp3FrameCount, {0ULL, 0ULL, 0, 0});

    if (!drmp3_calculate_seek_points(&this->mp3, &mp3FrameInt, seekTable.data()))
    {
        drmp3_uninit(&this->mp3);
        throw love::Exception("Could not calculate seek table.");
    }
    mp3FrameInt = (mp3FrameInt > mp3FrameCount) ? mp3FrameCount : mp3FrameInt;

    if (!drmp3_bind_seek_table(&this->mp3, mp3FrameInt, seekTable.data()))
    {
        drmp3_uninit(&this->mp3);
        throw love::Exception("Could not bind seek table");
    }
}

MPEGDecoder::~MPEGDecoder()
{
    drmp3_uninit(&this->mp3);
}

bool MPEGDecoder::Accepts(const std::string & ext)
{
    static const std::string supported[] = { "mp3", "" };

    for (int i = 0; !(supported[i].empty()); i++)
    {
        if (supported[i].compare(ext) == 0)
            return true;
    }

    return false;
}

Decoder * MPEGDecoder::Clone()
{
    return new MPEGDecoder(this->data, bufferSize);
}

int MPEGDecoder::Decode()
{
    return this->Decode((s16 *)this->buffer);
}

int MPEGDecoder::Decode(s16 * buffer)
{
    int maxRead = this->bufferSize / sizeof(int16_t) / this->mp3.channels;
    int read = (int)drmp3_read_pcm_frames_s16(&this->mp3, maxRead, (drmp3_int16 *)buffer);

    if (read < maxRead)
        this->eof = true;

    return read * sizeof(int16_t) * mp3.channels;
}

bool MPEGDecoder::Seek(double position)
{
    drmp3_uint64 targetSample = position * this->mp3.sampleRate;
    drmp3_bool32 success = drmp3_seek_to_pcm_frame(&this->mp3, targetSample);

    if (success)
        this->eof = true;

    return success;
}

bool MPEGDecoder::Rewind()
{
    return this->Seek(0.0);
}

bool MPEGDecoder::IsSeekable()
{
    return true;
}

int MPEGDecoder::GetChannelCount() const
{
    return this->mp3.channels;
}

int MPEGDecoder::GetBitDepth() const
{
    return 16;
}

double MPEGDecoder::GetDuration()
{
    return duration;
}
