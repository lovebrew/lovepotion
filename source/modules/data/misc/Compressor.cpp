#include "modules/data/misc/LZ4Compressor.hpp"
#include "modules/data/misc/ZlibCompressor.hpp"

namespace love
{
    Compressor* Compressor::getCompressor(Compressor::Format format)
    {
        static LZ4Compressor lz4;
        static ZlibCompressor zlib;

        Compressor* compressors[] = { &lz4, &zlib };
        for (auto* compressor : compressors)
        {
            if (compressor->isSupported(format))
                return compressor;
        }

        return nullptr;
    }
} // namespace love
