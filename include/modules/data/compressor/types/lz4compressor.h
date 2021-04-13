#pragma once

#include "modules/data/compressor/compressor.h"

namespace love
{
    class LZ4Compressor : public Compressor
    {
      public:
        char* Compress(Compressor::Format format, const char* data, size_t size, int level,
                       size_t& compressedSize) override
        {
            if (format != Compressor::FORMAT_LZ4)
                throw love::Exception("Invalid format (expected LZ4)");

            if (size > LZ4_MAX_INPUT_SIZE)
                throw love::Exception("Data is too large for LZ4 compressor");

            // LÖVE uses a custom header
            const size_t headerSize = sizeof(uint32_t);

            int maxDestinationSize = LZ4_compressBound((int)size);
            size_t maxSize         = headerSize + (size_t)maxDestinationSize;

            char* compressedBytes = nullptr;

            try
            {
                compressedBytes = new char[maxSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }

            *(uint32_t*)compressedBytes = (uint32_t)size;

            int compressionSize = 0;

            if (level > 8)
                compressionSize = LZ4_compress_HC(data, compressedBytes + headerSize, (int)size,
                                                  maxDestinationSize, LZ4HC_CLEVEL_DEFAULT);
            else
                compressionSize = LZ4_compress_default(data, compressedBytes + headerSize,
                                                       (int)size, maxDestinationSize);

            if (compressionSize <= 0)
            {
                delete[] compressedBytes;
                throw love::Exception("Could not LZ4-compress data!");
            }

            /*
            ** We allocated space for the maximum possible amount of data, but the
            ** actual compressed size might be much smaller, so we should shrink the
            ** data buffer if so.
            */

            if ((double)maxSize / (double)(compressionSize + headerSize) > 1.2)
            {
                char* compressionBytes = new (std::nothrow) char[compressionSize + headerSize];

                if (compressionBytes)
                {
                    memcpy(compressionBytes, compressedBytes, compressionSize + headerSize);
                    delete[] compressedBytes;
                    compressedBytes = compressionBytes;
                }
            }

            compressedSize = (size_t)compressionSize + headerSize;

            return compressedBytes;
        }

        char* Decompress(Compressor::Format format, const char* data, size_t size,
                         size_t& decompressedSize) override
        {
            if (format != FORMAT_LZ4)
                throw love::Exception("Invalid format (expected LZ4).");

            // LÖVE uses a custom header
            const size_t headerSize = sizeof(uint32_t);

            char* rawBytes = nullptr;

            if (size < headerSize)
                throw love::Exception("Invalid LZ4-compressed data size.");

            uint32_t rawSize = *(uint32_t*)data;

            try
            {
                rawBytes = new char[rawSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }

            /*
            ** If the uncompressed size is passed in as an argument (non-zero) and
            ** it matches the header's stored size, then we assume it's 100% accurate
            ** and we use a more efficient decompression function.
            */

            if (decompressedSize > 0 && decompressedSize == (size_t)rawSize)
            {
                // We don't care for the header here, but account for its size
                if (LZ4_decompress_fast(data + headerSize, rawBytes, (int)decompressedSize) < 0)
                {
                    delete[] rawBytes;
                    throw love::Exception("Could not decompress LZ4-compressed data!");
                }
            }
            else
            {
                // We do care for the header here, account for its size
                int result = LZ4_decompress_safe(data + headerSize, rawBytes,
                                                 (int)(size - headerSize), rawSize);

                if (result < 0)
                {
                    delete[] rawBytes;
                    throw love::Exception("Could not decompress LZ4-compressed data.");
                }

                decompressedSize = (size_t)result;
            }

            return rawBytes;
        }

        bool IsSupported(Compressor::Format format) const
        {
            return format == Compressor::FORMAT_LZ4;
        }
    };
} // namespace love
