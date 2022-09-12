#pragma once

#include <utilities/decoder/decoder.hpp>

#include <wuff.h>

namespace love
{
    class WaveDecoder : public Decoder
    {
      public:
        WaveDecoder(Stream* stream, int bufferSize);

        ~WaveDecoder();

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
        wuff_handle* handle;
        wuff_info info;
    };
} // namespace love
