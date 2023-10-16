#pragma once

#include <dr_flac.h>

#include <utilities/decoder/decoder.hpp>

namespace love
{
    class FLACDecoder : public Decoder
    {
      public:
        FLACDecoder(Stream* stream, int bufferSize);

        ~FLACDecoder();

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
        drflac* handle;
    };
} // namespace love
