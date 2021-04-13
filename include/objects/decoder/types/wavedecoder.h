#pragma once

#include "wuff.h"

#include "objects/decoder/decoder.h"

namespace love
{
    class WaveDecoder : public Decoder
    {
      public:
        struct WaveFile
        {
            char* data;
            size_t size;
            size_t offset;
        };

        WaveDecoder(Data* decoder, int bufferCode);
        ~WaveDecoder();

        static bool Accepts(const std::string& ext);

        Decoder* Clone();

        int Decode();

        int Decode(s16* buffer);

        bool Seek(double position);

        bool Rewind();

        bool IsSeekable();

        int GetChannelCount() const;

        int GetBitDepth() const;

        int GetSampleRate() const;

        double GetDuration();

      private:
        WaveFile file;

        wuff_handle* handle;
        wuff_info info;
    };
} // namespace love
