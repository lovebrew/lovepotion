#pragma once

#include "common/data.h"
#include "common/exception.h"

#include "common/strongref.h"

namespace love
{
    class Decoder : public Object
    {
      public:
        static love::Type type;

        Decoder(Data* data, int bufferSize);

        virtual ~Decoder();

        static const int DEFAULT_BUFFER_SIZE = 0x4000;
        static const int DEFAULT_SAMPLE_RATE = 44100;

        static const int DEFAULT_CHANNELS  = 2;
        static const int DEFAULT_BIT_DEPTH = 16;

        virtual Decoder* Clone() = 0;

        virtual int Decode() = 0;

        virtual int Decode(s16* buffer) = 0;

        virtual int GetSize() const;

        virtual void* GetBuffer() const;

        virtual bool Seek(double position) = 0;

        virtual bool Rewind() = 0;

        virtual bool IsSeekable() = 0;

        virtual bool IsFinished();

        virtual int GetChannelCount() const = 0;

        virtual int GetBitDepth() const = 0;

        virtual int GetSampleRate() const;

        virtual double GetDuration() = 0;

      protected:
        StrongReference<Data> data;

        int bufferSize;
        int sampleRate;
        void* buffer;
        bool eof;
    };
} // namespace love
