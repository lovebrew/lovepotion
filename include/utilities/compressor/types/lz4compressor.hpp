#pragma once

#include <utilities/compressor/compressor.hpp>

namespace love
{
    class LZ4Compressor : public Compressor
    {
      public:
        char* Compress(Compressor::Format format, const char* data, size_t size,
                       int level, size_t& compressedSize) override;

        char* Decompress(Compressor::Format format, const char* data, size_t size,
                         size_t& decompressedSize) override;

        bool IsSupported(Compressor::Format format) const
        {
            return format == Compressor::FORMAT_LZ4;
        }
    };
} // namespace love
