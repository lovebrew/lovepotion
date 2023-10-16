#pragma once

#include <utilities/decoder/decoder.hpp>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

namespace love
{
    class VorbisDecoder : public Decoder
    {
      public:
        VorbisDecoder(Stream* stream, int bufferSize);

        virtual ~VorbisDecoder();

        Decoder* Clone() override;

        int Decode() override;

        bool Seek(double position) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        int GetSampleRate() const override;

        double GetDuration() override;

      private:
        OggVorbis_File handle;
        vorbis_info* info;

        double duration;
    };
} // namespace love
