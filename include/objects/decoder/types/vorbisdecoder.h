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
        VorbisDecoder(Stream* stream, int bufferSize);

        ~VorbisDecoder();

        Decoder* Clone() override;

        int Decode(s16* buffer) override;

        int Decode() override;

        bool Seek(double s) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        int GetSampleRate() const override;

        double GetDuration() override;

        static int Probe(Stream* stream);

      private:
        static constexpr int HEADER_SIZE        = 0x04;
        static constexpr const char* HEADER_TAG = "OggS";

        OggVorbis_File handle;
        vorbis_info* info;

        double duration;
    };
} // namespace love
