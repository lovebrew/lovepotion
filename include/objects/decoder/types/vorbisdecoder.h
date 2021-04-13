#pragma once

#define OV_EXCLUDE_STATIC_CALLBACKS

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include "objects/decoder/decoder.h"

namespace love
{
    class VorbisDecoder : public Decoder
    {
      public:
        struct OggFile
        {
            const char* data;
            int64_t size;
            int64_t read;
        };

        VorbisDecoder(Data* data, int bufferSize);
        ~VorbisDecoder();

        static bool Accepts(const std::string& extension);

        Decoder* Clone();

        int Decode(s16* buffer);

        int Decode();

        bool Seek(double s);

        bool Rewind();

        bool IsSeekable();

        int GetChannelCount() const;

        int GetBitDepth() const;

        int GetSampleRate() const;

        double GetDuration();

      private:
        OggFile file;

        ov_callbacks callbacks;
        OggVorbis_File handle;
        vorbis_info* info;
        vorbis_comment* comment;

        double duration;
    };
} // namespace love
