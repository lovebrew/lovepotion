#pragma once

#include <utilities/compressor/compressor.hpp>

namespace love
{
    class ZlibCompressor : public Compressor
    {
      public:
        char* Compress(Compressor::Format format, const char* data, size_t size,
                       int level, size_t& compressedSize) override;

        char* Decompress(Compressor::Format format, const char* data, size_t size,
                         size_t& decompressedSize) override;

        bool IsSupported(Compressor::Format format) const
        {
            return format == Compressor::FORMAT_ZLIB ||
                   format == Compressor::FORMAT_GZIP ||
                   format == Compressor::FORMAT_DEFLATE;
        }

      private:
        uLong zlibCompressBound(Compressor::Format format, uLong sourceLength);

        int zlibCompress(Compressor::Format format, Bytef* destination,
                         uLongf* destinationLength, const Bytef* source,
                         uLong sourceLength, int level);

        int zlibDecompress(Compressor::Format format, Bytef* destination,
                           uLongf* destinationLength, const Bytef* source,
                           uLong sourceLength);
    };
} // namespace love
