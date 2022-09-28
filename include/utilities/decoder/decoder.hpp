#pragma once

#include <common/object.hpp>
#include <common/strongreference.hpp>

#include <utilities/stream/stream.hpp>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <memory>
#include <vector>

namespace love
{
    class Decoder : public Object
    {
      public:
        enum StreamSource
        {
            STREAM_FILE,
            STREAM_MEMORY,
            STREAM_MAX_ENUM
        };

        static constexpr int DEFAULT_BUFFER_SIZE = 0x4000;
        static constexpr int DEFAULT_SAMPLE_RATE = 0xAC44;

        static constexpr int DEFAULT_CHANNELS  = 0x02;
        static constexpr int DEFAULT_BIT_DEPTH = 0x10;

        static Type type;

        Decoder(Stream* stream, int bufferSize);

        virtual Decoder* Clone() = 0;

        virtual int Decode() = 0;

        virtual void* GetBuffer() const;

        virtual int GetBitDepth() const = 0;

        virtual int GetChannelCount() const = 0;

        virtual double GetDuration() = 0;

        virtual int GetSampleRate() const;

        virtual int GetSize() const;

        virtual bool IsFinished();

        virtual bool IsSeekable() = 0;

        virtual bool Rewind() = 0;

        virtual bool Seek(double position) = 0;

        // clang-format off
        static constexpr BidirectionalMap streamSources = {
            "memory", Decoder::STREAM_MEMORY,
            "file",   Decoder::STREAM_FILE
        };
        // clang-format on

      protected:
        StrongReference<Stream> stream;

        int bufferSize;
        int sampleRate;

        std::unique_ptr<uint8_t[]> buffer;
        bool eof;
    };
} // namespace love
