#pragma once

#include "common/Stream.hpp"
#include "modules/sound/Decoder.hpp"

#include <libmodplug/modplug.h>

namespace love
{
    class ModPlugDecoder : public Decoder
    {
      public:
        ModPlugDecoder(Stream* stream, int bufferSize);

        virtual ~ModPlugDecoder();

        virtual Decoder* clone() override;

        virtual int decode() override;

        virtual bool seek(double position) override;

        virtual bool rewind() override;

        virtual bool isSeekable() override;

        virtual int getChannelCount() const override;

        virtual int getBitDepth() const override;

        virtual double getDuration() override;

      private:
        StrongRef<Data> data;

        ModPlugFile* handle;
        ModPlug_Settings settings;

        double duration;
    };
} // namespace love
