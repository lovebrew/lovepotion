#pragma once

#include <common/console.hpp>
#include <common/exception.hpp>
#include <common/object.hpp>

#include <vector>

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

        static bool GetConstant(const char* in, SourceType& out);
        static bool GetConstant(SourceType in, const char*& out);
        static std::vector<const char*> GetConstants(SourceType);

        static bool GetConstant(const char* in, Unit& out);
        static bool GetConstant(Unit in, const char*& out);
        static std::vector<const char*> GetConstants(Unit);

      protected:
        SourceType sourceType;
    };
} // namespace love
