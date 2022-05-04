#pragma once

#include <mpg123.h>

#include "objects/decoder/decoder.h"

namespace love
{
    class MP3Decoder : public Decoder
    {
      public:
        MP3Decoder(Stream* data, int bufferSize);

        ~MP3Decoder();

        static void Quit();

        Decoder* Clone() override;

        int Decode() override;

        int Decode(s16* buffer) override;

        bool Seek(double position) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        double GetDuration() override;

      private:
        mpg123_handle* handle;
        static bool inited;

        int channels;
        double duration;
    };
} // namespace love
