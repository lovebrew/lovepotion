#pragma once

#include "common/Stream.hpp"
#include "common/int.hpp"
#include "modules/sound/Decoder.hpp"

#define OV_EXCLUDE_STATIC_CALLBACKS
#if !defined(__WIIU__)
    #include <tremor/ivorbiscodec.h>
    #include <tremor/ivorbisfile.h>
#else
    #include <vorbis/codec.h>
    #include <vorbis/vorbisfile.h>
#endif

namespace love
{
    class VorbisDecoder : public Decoder
    {
      public:
        VorbisDecoder(Stream* stream, int bufferSize);

        virtual ~VorbisDecoder();

        Decoder* clone() override;

        virtual int decode() override;

        virtual bool seek(double position) override;

        virtual bool rewind() override;

        virtual bool isSeekable() override;

        virtual int getChannelCount() const override;

        virtual int getBitDepth() const override;

        virtual int getSampleRate() const override;

        virtual double getDuration() override;

      private:
        static constexpr double EPSILON = 0.000001;

        OggVorbis_File handle;
        vorbis_info* vorbisInfo;
        double duration;
    };
} // namespace love
