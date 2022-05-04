#pragma once

#include "objects/decoder/decoder.h"
#include <libmodplug/modplug.h>

namespace love
{
    class ModPlugDecoder : public Decoder
    {
      public:
        ModPlugDecoder(Stream* stream, int bufferSize);

        virtual ~ModPlugDecoder();

        Decoder* Clone() override;

        int Decode() override;

        int Decode(s16* buffer) override;

        bool Seek(double position) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        double GetDuration() override;

        static int Probe(Stream* stream);

      private:
        static constexpr int DATA_SIZE = 0x400000;

        StrongReference<Data> data;

        ModPlugFile* plug;
        ModPlug_Settings settings;

        double duration;
    };
} // namespace love
