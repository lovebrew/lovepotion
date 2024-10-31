#pragma once

#include "modules/data/misc/Compressor.hpp"

#include "common/Exception.hpp"
#include "common/int.hpp"

#include <cstring>

#include <zlib.h>

namespace love
{
    class ZlibCompressor : public Compressor
    {
      private:
        uLong zlibCompressBound(Format format, uLong sourceLen)
        {
            uLong size = sourceLen + (sourceLen >> 12) + (sourceLen >> 14) + (sourceLen >> 25) + 13;

            if (format == FORMAT_GZIP)
                size += 18 - 6;

            return size;
        }

        int zlibCompress(Format format, Bytef* destination, uLongf* destinationLength, const Bytef* source,
                         uLong sourceLength, int level)
        {
            z_stream stream {};

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

        int zlibDecompress(Format format, Bytef* destination, uLongf* destinationLength, const Bytef* source,
                           uLong sourceLength)
        {
            z_stream stream {};

            stream.next_in  = (Bytef*)source;
            stream.avail_in = (uInt)sourceLength;

            stream.next_out  = destination;
            stream.avail_out = (uInt)(*destinationLength);

            int windowBits = 15 + 32;
            if (format == FORMAT_DEFLATE)
                windowBits = -15;

            int error = inflateInit2(&stream, windowBits);

            if (error != Z_OK)
                return error;

            error = inflate(&stream, Z_FINISH);

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

      public:
        char* compress(Compressor::Format format, const char* data, size_t dataSize, int level,
                       size_t& compressedSize) override
        {
            if (!this->isSupported(format))
                throw love::Exception(E_INVALID_COMPRESSION_FORMAT_ZLIB);

            if (level < 0)
                level = Z_DEFAULT_COMPRESSION;
            else if (level > 9)
                level = 9;

            uLong maxSize = zlibCompressBound(format, (uLong)dataSize);

            char* compressedBytes = nullptr;

            try
            {
                compressedBytes = new char[maxSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception(E_OUT_OF_MEMORY);
            }

            uLongf dstLength = (uLongf)maxSize;
            int status       = zlibCompress(format, (Bytef*)compressedBytes, &dstLength, (const Bytef*)data,
                                            (uLong)dataSize, level);

            if (status != Z_OK)
            {
                delete[] compressedBytes;
                throw love::Exception("Could not zlib/gzip-compress data.");
            }

            if ((double)maxSize / (double)dstLength >= 1.3)
            {
                char* bytes = new (std::nothrow) char[dstLength];

                if (bytes)
                {
                    std::memcpy(bytes, compressedBytes, dstLength);
                    delete[] compressedBytes;
                    compressedBytes = bytes;
                }
            }

            compressedSize = (size_t)dstLength;
            return compressedBytes;
        }

        char* decompress(Compressor::Format format, const char* data, size_t dataSize,
                         size_t& decompressedSize) override
        {
            if (!this->isSupported(format))
                throw love::Exception(E_INVALID_COMPRESSION_FORMAT_ZLIB);

            char* rawBytes = nullptr;
            size_t rawSize = decompressedSize > 0 ? decompressedSize : dataSize * 2;

            while (true)
            {
                try
                {
                    rawBytes = new char[rawSize];
                }
                catch (std::bad_alloc&)
                {
                    throw love::Exception(E_OUT_OF_MEMORY);
                }

                uLongf dstLength = (uLongf)rawSize;
                int status =
                    zlibDecompress(format, (Bytef*)rawBytes, &dstLength, (const Bytef*)data, (uLong)dataSize);

                if (status == Z_OK)
                {
                    decompressedSize = (size_t)dstLength;
                    break;
                }
                else if (status != Z_BUF_ERROR)
                {
                    delete[] rawBytes;
                    throw love::Exception("Could not decompress zlib/gzip-compressed data.");
                }

                delete[] rawBytes;
                rawSize *= 2;
            }

            return rawBytes;
        }

        bool isSupported(Compressor::Format format) const override
        {
            return format == Compressor::FORMAT_GZIP || format == Compressor::FORMAT_ZLIB ||
                   format == Compressor::FORMAT_DEFLATE;
        }
    };
} // namespace love
