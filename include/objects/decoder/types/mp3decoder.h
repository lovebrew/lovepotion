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

        static int Probe(Stream* stream);

      private:
        static constexpr int MIN_HEADER_SIZE = 0xA;

        static constexpr const char* ID3V1_TAG = "TAG";
        static constexpr int ID3V1_SIZE        = 0x80;

        static constexpr const char* ID3V2_TAG = "ID3";

        mpg123_handle* handle;
        static bool inited;

        int channels;
        double duration;
    };
} // namespace love
