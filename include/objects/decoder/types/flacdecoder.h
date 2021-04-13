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
            int sampleRate;
            int channels;
            int PCMCount;
            int bitsPerSample;
            int totalSamples;

            const char* data;
            size_t size;
            size_t read;

            size_t usedSamples;

            int32_t* buffer;
            int32_t* writePointer = nullptr;
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
        static bool inited;

        FLACFile file;

        FLAC__StreamDecoder* decoder;
        FLAC__StreamDecoderInitStatus status;
    };
} // namespace love
