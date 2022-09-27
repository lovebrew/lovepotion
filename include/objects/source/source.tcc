#pragma once

#include <common/console.hpp>
#include <common/exception.hpp>
#include <common/object.hpp>

#include <vector>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    class InvalidFormatException : public Exception
    {
      public:
        InvalidFormatException(int channels, int bitDepth) :
            Exception("%d-channel Sources with %d bits per sample are not supported.", channels,
                      bitDepth)
        {}
    };

    class QueueFormatMismatchException : public Exception
    {
      public:
        QueueFormatMismatchException() :
            Exception("Queued sound data must have same format as sound Source.")
        {}
    };

    class QueueTypeMismatchException : public love::Exception
    {
      public:
        QueueTypeMismatchException() :
            Exception("Only queueable Sources can be queued with sound data.")
        {}
    };

    class QueueMalformedLengthException : public love::Exception
    {
      public:
        QueueMalformedLengthException(int bytes) :
            Exception("Data length must be a multiple of sample size (%d bytes).", bytes)
        {}
    };

    class QueueLoopingException : public love::Exception
    {
      public:
        QueueLoopingException() : Exception("Queueable Sources can not be looped.")
        {}
    };

    template<Console::Platform T = Console::ALL>
    class Source : public Object
    {
      public:
        class DataBuffer
        {
          public:
            DataBuffer()
            {}

            DataBuffer(const void* data, size_t size);

            ~DataBuffer();

            int16_t* GetBuffer()
            {
                return this->buffer;
            }

            size_t GetSize()
            {
                return this->size;
            }

          private:
            int16_t* buffer;
            size_t size;
        };

        enum SourceType
        {
            TYPE_STATIC,
            TYPE_STREAM,
            TYPE_QUEUE,
            TYPE_MAX_ENUM
        };

        enum Unit
        {
            UNIT_SECONDS,
            UNIT_SAMPLES,
            UNIT_MAX_ENUM
        };

        static inline Type type = Type("Source", &Object::type);

        Source(SourceType type) : sourceType(type)
        {}

        SourceType GetType() const
        {
            return this->sourceType;
        }

        float GetMaxVolume() const
        {
            return this->maxVolume;
        }

        void SetMaxVolume(float volume)
        {
            this->maxVolume = volume;
        }

        float GetMinVolume() const
        {
            return this->minVolume;
        }

        void SetMinVolume(float volume)
        {
            this->minVolume = volume;
        }

        // clang-format off
        static constexpr BidirectionalMap sourceTypes = {
          "static", TYPE_STATIC,
          "stream", TYPE_STREAM,
          "queue",  TYPE_QUEUE
        };
        // clang-format on

        static bool GetConstant(const char* in, SourceType& out)
        {
            return sourceTypes.Find(in, out);
        }
        static bool GetConstant(SourceType in, const char*& out)
        {
            return sourceTypes.ReverseFind(in, out);
        }

        static SmallTrivialVector<const char*, 3> GetConstants(SourceType)
        {
            return sourceTypes.GetNames();
        }

        // clang-format off
        static constexpr BidirectionalMap unitTypes = {
          "seconds", UNIT_SECONDS,
          "samples", UNIT_SAMPLES
        };
        // clang-format on

        static bool GetConstant(const char* in, Unit& out)
        {
            return unitTypes.Find(in, out);
        }

        static bool GetConstant(Unit in, const char*& out)
        {
            return unitTypes.ReverseFind(in, out);
        }

        static SmallTrivialVector<const char*, 2> GetConstants(Unit)
        {
            return unitTypes.GetNames();
        }

      protected:
        SourceType sourceType;
        bool looping;

        float minVolume;
        float maxVolume;
        float volume;
    };
} // namespace love
