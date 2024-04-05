#pragma once

#include "modules/data/misc/Compressor.hpp"

#include "common/Exception.hpp"
#include "common/int.hpp"

#include <cstring>

#include <lz4.h>
#include <lz4hc.h>

namespace love
{
    class LZ4Compressor : public Compressor
    {
      public:
        char* compress(Compressor::Format format, const char* data, size_t dataSize, int level,
                       size_t& compressedSize) override
        {
            if (format != Compressor::FORMAT_LZ4)
                throw love::Exception(E_INVALID_COMPRESSION_FORMAT_LZ4);

            if (dataSize > LZ4_MAX_INPUT_SIZE)
                throw love::Exception("Data is too large for LZ4 compressor.");

            const size_t headerSize = sizeof(uint32_t);

            int maxDestinationSize = LZ4_compressBound((int)dataSize);
            size_t maxSize         = headerSize + (size_t)maxDestinationSize;

            char* compressedBytes = nullptr;

            try
            {
                compressedBytes = new char[maxSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception(E_OUT_OF_MEMORY);
            }

#if defined(LOVE_BIG_ENDIAN)
            *(uint32_t*)compressedBytes = swap_uint32((uint32_t)dataSize);
#else
            *(uint32_t*)compressedBytes = (uint32_t)dataSize;
#endif

            int compSize = 0;

            // clang-format off
            if (level > 8)
                compSize = LZ4_compress_HC(data, compressedBytes + headerSize, (int)dataSize, (int)maxDestinationSize, LZ4HC_CLEVEL_DEFAULT);
            else
                compSize = LZ4_compress_default(data, compressedBytes + headerSize, (int)dataSize, (int)maxDestinationSize);
            // clang-format on

            if (compSize <= 0)
            {
                delete[] compressedBytes;
                throw love::Exception("Could not LZ4-compress data.");
            }

            if ((double)maxSize / (double)(compSize + headerSize) >= 1.2)
            {
                char* bytes = new (std::nothrow) char[compSize + headerSize];

                if (bytes)
                {
                    std::memcpy(bytes, compressedBytes, compSize + headerSize);
                    delete[] compressedBytes;
                    compressedBytes = bytes;
                }
            }

            compressedSize = (size_t)compSize + headerSize;

            return compressedBytes;
        }

        char* decompress(Compressor::Format format, const char* data, size_t dataSize,
                         size_t& decompressedSize) override
        {
            if (format != Compressor::FORMAT_LZ4)
                throw love::Exception(E_INVALID_COMPRESSION_FORMAT_LZ4);

            const size_t headerSize = sizeof(uint32_t);
            char* rawBytes          = nullptr;

            if (dataSize < headerSize)
                throw love::Exception("Invalid LZ4-compressed data size.");

#if defined(LOVE_BIG_ENDIAN)
            uint32_t rawSize = swap_uint32(*(uint32_t*)data);
#else
            uint32_t rawSize = *(uint32_t*)data;
#endif

            try
            {
                rawBytes = new char[rawSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception(E_OUT_OF_MEMORY);
            }

            if (decompressedSize > 0 && decompressedSize == (size_t)rawSize)
            {
                if (LZ4_decompress_fast(data + headerSize, rawBytes, (int)decompressedSize) < 0)
                {
                    delete[] rawBytes;
                    throw love::Exception(E_COULD_NOT_LZ4_DECOMPRESS_DATA);
                }
            }
            else
            {
                // clang-format off
                int result = LZ4_decompress_safe(data + headerSize, rawBytes, (int)dataSize - (int)headerSize, (int)rawSize);
                // clang-format on

                if (result < 0)
                {
                    delete[] rawBytes;
                    throw love::Exception(E_COULD_NOT_LZ4_DECOMPRESS_DATA);
                }

                decompressedSize = (size_t)result;
            }

            return rawBytes;
        }

        bool isSupported(Compressor::Format format) const override
        {
            return format == Compressor::FORMAT_LZ4;
        }
    };
} // namespace love
