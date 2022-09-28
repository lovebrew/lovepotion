#pragma once

#include <common/exception.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <lz4.h>
#include <lz4hc.h>

#include <zlib.h>

#include <vector>

namespace love
{
    class Compressor
    {
      public:
        enum Format
        {
            FORMAT_LZ4,
            FORMAT_ZLIB,
            FORMAT_GZIP,
            FORMAT_DEFLATE,
            FORMAT_MAX_ENUM
        };

        static Compressor* GetCompressor(Format format);

        virtual ~Compressor()
        {}

        virtual char* Compress(Format format, const char* data, size_t size, int level,
                               size_t& compressedSize) = 0;

        virtual char* Decompress(Format format, const char* data, size_t size,
                                 size_t& decompressedSize) = 0;

        virtual bool IsSupported(Format format) const = 0;

        // clang-format off
        static constexpr BidirectionalMap formats = {
            "lz4",     Compressor::Format::FORMAT_LZ4,
            "zlib",    Compressor::Format::FORMAT_ZLIB,
            "gzip",    Compressor::Format::FORMAT_GZIP,
            "deflate", Compressor::Format::FORMAT_DEFLATE
        };
        // clang-format on

      protected:
        Compressor()
        {}
    };
} // namespace love
