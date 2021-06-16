#pragma once

#include "objects/decoder/decoder.h"
#include <FLAC/stream_decoder.h>

namespace love
{
    class FLACDecoder : public Decoder
    {
      public:
        FLACDecoder(Data* data, int bufferSize);
        ~FLACDecoder();

        struct FLACFile
        {
            size_t bitsPerSample;
            size_t sampleRate;
            size_t channels;

            uint32_t totalSamples;

            const char* data;
            size_t size;
            size_t read;

            int32_t* outputBuffer;
            int32_t* writeBuffer;

            size_t bufferUsed;
        };

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
        FLACFile file;
        size_t decodeBufferRead;

        FLAC__StreamDecoder* decoder;
        FLAC__StreamDecoderInitStatus status;
    };
} // namespace love
