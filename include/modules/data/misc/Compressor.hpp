#pragma once

#include "common/Map.hpp"

#include <stddef.h>

namespace love
{
    class Compressor
    {
      public:
        enum Format
        {
            FORMAT_LZ4,
            FORMAT_GZIP,
            FORMAT_ZLIB,
            FORMAT_DEFLATE,
            FORMAT_MAX_ENUM
        };

        static Compressor* getCompressor(Format format);

        virtual ~Compressor()
        {}

        virtual char* compress(Format format, const char* data, size_t dataSize, int level,
                               size_t& compressedSize) = 0;

        virtual char* decompress(Format format, const char* data, size_t dataSize,
                                 size_t& decompressedSize) = 0;

        virtual bool isSupported(Format format) const = 0;

        // clang-format off
        STRINGMAP_DECLARE(formats, Format,
            { "lz4",     FORMAT_LZ4     },
            { "gzip",    FORMAT_GZIP    },
            { "zlib",    FORMAT_ZLIB    },
            { "deflate", FORMAT_DEFLATE }
        );
        // clang-format on

      protected:
        Compressor()
        {}
    };
} // namespace love
