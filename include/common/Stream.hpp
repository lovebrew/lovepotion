#pragma once

#include "common/Object.hpp"

#include <cstddef>

namespace love
{
    class Data;

    class Stream : public Object
    {
      public:
        enum SeekOrigin
        {
            SEEKORIGIN_BEGIN,
            SEEKORIGIN_CURRENT,
            SEEKORIGIN_END,
            SEEKORIGIN_MAX_ENUM
        };

        static Type type;

        virtual ~Stream()
        {}

        virtual Stream* clone() const = 0;

        Data* read(int64_t size);

        virtual int64_t read(void* destination, int64_t size) = 0;

        bool write(Data* source, int64_t offset, int64_t size);

        virtual bool write(const void* source, int64_t size) = 0;

        bool write(Data* source);

        virtual bool seek(int64_t position, SeekOrigin origin = SEEKORIGIN_BEGIN) = 0;

        virtual bool isSeekable() const = 0;

        virtual bool isWritable() const = 0;

        virtual bool isReadable() const = 0;

        virtual bool flush() = 0;

        virtual int64_t getSize() = 0;

        virtual int64_t tell() = 0;
    };
} // namespace love
