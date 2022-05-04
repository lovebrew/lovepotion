#pragma once

#include "wuff.h"

#include "objects/decoder/decoder.h"

namespace love
{
    class WaveDecoder : public Decoder
    {
      public:
        WaveDecoder(Stream* stream, int bufferCode);

        ~WaveDecoder();

        Decoder* Clone() override;

        int Decode() override;

        int Decode(s16* buffer) override;

        bool Seek(double position) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        int GetSampleRate() const override;

        double GetDuration() override;

        static int Probe(Stream* stream);

      private:
        static constexpr int HEADER_SIZE        = 0x08;
        static constexpr const char* HEADER_TAG = "RIFF";

        static constexpr const char* FORMAT_TAG = "WAVEfmt";

        wuff_handle* handle;
        wuff_info info;
    };
} // namespace love
