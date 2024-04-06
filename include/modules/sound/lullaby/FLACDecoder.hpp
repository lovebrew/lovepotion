#pragma once

#include "common/Stream.hpp"
#include "modules/sound/Decoder.hpp"

#include "dr_flac.h"
#include <string.h>

namespace love
{
    class FLACDecoder : public Decoder
    {
      public:
        FLACDecoder(Stream* stream, int bufferSize);

        virtual ~FLACDecoder();

        virtual Decoder* clone() override;

        virtual int decode() override;

        virtual bool seek(double s) override;

        virtual bool rewind() override;

        virtual bool isSeekable() override;

        virtual int getChannelCount() const override;

        virtual int getBitDepth() const override;

        virtual int getSampleRate() const override;

        virtual double getDuration() override;

      private:
        drflac* handle;
    };
} // namespace love
