#pragma once

#include <dr_mp3.h>

#include <utilities/decoder/decoder.hpp>

namespace love
{
    class MP3Decoder : public Decoder
    {
      public:
        MP3Decoder(Stream* data, int bufferSize);

        ~MP3Decoder();

        Decoder* Clone() override;

        int Decode() override;

        bool Seek(double position) override;

        bool Rewind() override;

        bool IsSeekable() override;

        int GetChannelCount() const override;

        int GetBitDepth() const override;

        double GetDuration() override;

      private:
        static size_t OnRead(void* source, void* data, size_t bytesToRead);
        static drmp3_bool32 OnSeek(void* source, int offset, drmp3_seek_origin origin);

        static constexpr int EXPECTED_BITRATE    = 0x0F;
        static constexpr int EXPECTED_SAMPLERATE = 0x03;

        drmp3 handle;
        std::vector<drmp3_seek_point> seekTable;

        int64_t offset;
        double duration;
    };
} // namespace love
