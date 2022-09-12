#pragma once

#include <libmodplug/modplug.h>

#include <utilities/decoder/decoder.hpp>

namespace love
{
    class ModPlugDecoder : public Decoder
    {
      public:
        ModPlugDecoder(Stream* stream, int bufferSize);

        ~ModPlugDecoder();

        Decoder* Clone() override;

        int Decode() override;

        bool Seek(double position) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        double GetDuration() override;

      private:
        StrongReference<Data> data;

        ModPlugFile* plug;
        ModPlug_Settings settings;

        double duration;
    };
} // namespace love
