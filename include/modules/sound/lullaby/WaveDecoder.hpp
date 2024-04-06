#pragma once

#include "modules/sound/Decoder.hpp"
#include "wuff.h"

namespace love
{
    class WaveDecoder : public Decoder
    {
      public:
        WaveDecoder(Stream* stream, int bufferSize);

        virtual ~WaveDecoder();

        virtual Decoder* clone() override;

        virtual int decode() override;

        virtual bool seek(double position) override;

        virtual bool rewind() override;

        virtual bool isSeekable() override;

        virtual int getChannelCount() const override;

        virtual int getBitDepth() const override;

        virtual int getSampleRate() const override;

        virtual double getDuration() override;

      private:
        wuff_handle* handle;
        wuff_info info;
    };
} // namespace love
