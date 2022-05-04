#pragma once

#include "objects/decoder/decoder.h"
#include <FLAC/stream_decoder.h>

namespace love
{
    class FLACDecoder : public Decoder
    {
      public:
        FLACDecoder(Stream* stream, int bufferSize);
        ~FLACDecoder();

        struct FLACFile
        {
            size_t bitsPerSample;
            size_t sampleRate;
            size_t channels;

            uint32_t totalSamples;

            Stream* stream;
            size_t size;
            size_t read;

            int32_t* outputBuffer;
            int32_t* writeBuffer;

            size_t bufferUsed;
        };

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

      private:
        FLACFile file;
        size_t decodeBufferRead;

        FLAC__StreamDecoder* decoder;
        FLAC__StreamDecoderInitStatus status;
    };
} // namespace love
