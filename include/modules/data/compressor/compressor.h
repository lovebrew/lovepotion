#pragma once

#include "common/exception.h"

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

        static bool GetConstant(const char* in, Format& out);
        static bool GetConstant(Format in, const char*& out);
        static std::vector<const char*> GetConstants(Format);

      protected:
        Compressor()
        {}
    };
} // namespace love
