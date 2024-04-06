#pragma once

#include "common/Map.hpp"
#include "common/Object.hpp"
#include "common/Stream.hpp"
#include "common/StrongRef.hpp"

namespace love
{
    class Decoder : public Object
    {
      public:
        enum StreamSource
        {
            STREAM_MEMORY,
            STREAM_FILE,
            STREAM_MAX_ENUM
        };

        static Type type;

        Decoder(Stream* stream, int bufferSize);

        virtual ~Decoder();

        static constexpr int DEFAULT_BUFFER_SIZE = 16384;
        static constexpr int DEFAULT_SAMPLE_RATE = 44100;
        static constexpr int DEFAULT_CHANNELS    = 2;
        static constexpr int DEFAULT_BIT_DEPTH   = 16;

        virtual Decoder* clone() = 0;

        virtual int decode() = 0;

        virtual int getSize() const;

        virtual void* getBuffer() const;

        virtual bool seek(double position) = 0;

        virtual bool rewind() = 0;

        virtual bool isSeekable() = 0;

        virtual bool isFinished();

        virtual int getChannelCount() const = 0;

        virtual int getBitDepth() const = 0;

        virtual int getSampleRate() const;

        virtual double getDuration() = 0;

        // clang-format off
        STRINGMAP_DECLARE(StreamSources, StreamSource,
          { "memory", STREAM_MEMORY },
          { "file",   STREAM_FILE   }
        );
        // clang-format on

      protected:
        StrongRef<Stream> stream;
        int bufferSize;
        int sampleRate;
        void* buffer;
        bool eof;
    };
} // namespace love
