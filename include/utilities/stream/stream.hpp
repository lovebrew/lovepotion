#pragma once

#include <common/object.hpp>

#include <stddef.h>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    class Data;

    class Stream : public Object
    {
      public:
        enum SeekOrigin
        {
            ORIGIN_BEGIN,
            ORIGIN_CURRENT,
            ORIGIN_END,
            ORIGIN_MAX_ENUM
        };

        static love::Type type;

        virtual ~Stream()
        {}

        virtual Stream* Clone() = 0;

        virtual bool IsReadable() const = 0;

        virtual bool IsWritable() const = 0;

        virtual bool IsSeekable() const = 0;

        virtual int64_t Read(void* destination, int64_t size) = 0;

        virtual Data* Read(int64_t size);

        virtual bool Write(const void* source, int64_t size) = 0;

        virtual bool Write(Data* source, int64_t offset, int64_t size);

        bool Write(Data* source);

        virtual bool Flush() = 0;

        virtual int64_t GetSize() = 0;

        virtual bool Seek(int64_t position, SeekOrigin = ORIGIN_BEGIN) = 0;

        virtual int64_t Tell() = 0;

        // clang-format off
        static constexpr BidirectionalMap seekOrigins = {
            SEEK_SET, Stream::ORIGIN_BEGIN,
            SEEK_CUR, Stream::ORIGIN_CURRENT,
            SEEK_END, Stream::ORIGIN_END
        };
        // clang-format on
    };
} // namespace love
