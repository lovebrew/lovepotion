#pragma once

#include "modules/data/compressor/compressor.h"

namespace love
{
    class zlibCompressor : public Compressor
    {
      public:
        char* Compress(Compressor::Format format, const char* data, size_t size, int level,
                       size_t& compressedSize) override
        {
            if (!this->IsSupported(format))
                throw love::Exception("Invalid format (expected zlib or gzip).");

            if (level < 0)
                level = Z_DEFAULT_COMPRESSION;
            else if (level > 9)
                level = 9;

            uLong maxSize = this->zlibCompressBound(format, (uLong)size);

            char* compressedBytes = nullptr;

            try
            {
                compressedBytes = new char[maxSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }

            uLongf destinationLength = maxSize;

            int status = this->zlibCompress(format, (Bytef*)compressedBytes, &destinationLength,
                                            (const Bytef*)data, (uLong)size, level);

            if (status != Z_OK)
            {
                delete[] compressedBytes;
                throw love::Exception("Could not zlib/gzip-compress data.");
            }

            /*
            ** We allocated space for the maximum possible amount of data, but the
            ** actual compressed size might be much smaller, so we should shrink the
            ** data buffer if so.
            */

            if ((double)maxSize / (double)destinationLength >= 1.3)
            {
                char* compressionBytes = new (std::nothrow) char[destinationLength];

                if (compressionBytes)
                {
                    memcpy(compressionBytes, compressedBytes, destinationLength);
                    delete[] compressedBytes;
                    compressedBytes = compressionBytes;
                }
            }

            compressedSize = (size_t)destinationLength;

            return compressedBytes;
        }

        char* Decompress(Compressor::Format format, const char* data, size_t size,
                         size_t& decompressedSize) override
        {
            if (!this->IsSupported(format))
                throw love::Exception("Invalid format (expected zlib or gzip).");

            char* rawBytes = nullptr;

            // We might know the output size before decompression. If not, we guess.
            size_t rawSize = decompressedSize > 0 ? decompressedSize : size * 2;

            // Repeatedly try to decompress with an increasingly large output buffer.
            while (true)
            {
                try
                {
                    rawBytes = new char[rawSize];
                }
                catch (std::bad_alloc&)
                {
                    throw love::Exception("Out of memory.");
                }

                uLongf destinationLength = (uLongf)rawSize;

                int status = this->zlibDecompress(format, (Bytef*)rawBytes, &destinationLength,
                                                  (const Bytef*)data, (uLong)size);

                if (status == Z_OK)
                {
                    decompressedSize = (size_t)destinationLength;
                    break;
                }
                else if (status != Z_BUF_ERROR)
                {
                    // Anything except not enough room, throw an exception
                    delete[] rawBytes;
                    throw love::Exception("Could not decompress zlib/gzip-compressed data.");
                }

                // Not enough room: try with a larger size
                delete[] rawBytes;
                rawSize *= 2;
            }

            return rawBytes;
        }

        bool IsSupported(Compressor::Format format) const
        {
            return format == Compressor::FORMAT_ZLIB || format == Compressor::FORMAT_GZIP ||
                   format == Compressor::FORMAT_DEFLATE;
        }

      private:
        uLong zlibCompressBound(Compressor::Format format, uLong sourceLength)
        {
            uLong size = sourceLength + (sourceLength >> 12) + (sourceLength >> 14) +
                         (sourceLength >> 25) + 13;

            if (format == FORMAT_GZIP)
                size += 18 - 6;

            return size;
        }

        int zlibCompress(Compressor::Format format, Bytef* destination, uLongf* destinationLength,
                         const Bytef* source, uLong sourceLength, int level)
        {
            z_stream stream = {};

            stream.next_in  = (Bytef*)source;
            stream.avail_in = (uInt)sourceLength;

            stream.next_out  = destination;
            stream.avail_out = (uInt)(*destinationLength);

            int windowBits = 15;

            if (format == FORMAT_GZIP)
                windowBits += 16;
            else if (format == FORMAT_DEFLATE)
                windowBits = -windowBits;

            int error = deflateInit2(&stream, level, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY);

            if (error != Z_OK)
                return error;

            error = deflate(&stream, Z_FINISH);

            if (error != Z_STREAM_END)
            {
                deflateEnd(&stream);
                return error == Z_OK ? Z_BUF_ERROR : error;
            }

            *destinationLength = stream.total_out;

            return deflateEnd(&stream);
        }

        int zlibDecompress(Compressor::Format format, Bytef* destination, uLongf* destinationLength,
                           const Bytef* source, uLong sourceLength)
        {
            z_stream stream = {};

            stream.next_in  = (Bytef*)source;
            stream.avail_in = (uInt)sourceLength;

            stream.next_out  = destination;
            stream.avail_out = (uInt)(*destinationLength);

            // 15 is the default, add 32 to auto-detect header type
            int windowBits = 15 + 32;

            if (format == FORMAT_DEFLATE)
                windowBits = -15;

            int error = inflateInit2(&stream, windowBits);

            if (error != Z_OK)
                return error;

            error = inflate(&stream, windowBits);

            if (error != Z_STREAM_END)
            {
                inflateEnd(&stream);

                if (error == Z_NEED_DICT || (error == Z_BUF_ERROR && stream.avail_in == 0))
                    return Z_DATA_ERROR;

                return error;
            }

            *destinationLength = stream.total_out;

            return inflateEnd(&stream);
        }
    };
} // namespace love
