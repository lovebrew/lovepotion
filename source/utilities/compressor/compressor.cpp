#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/compressor/compressor.hpp>
#include <utilities/compressor/types/lz4compressor.hpp>
#include <utilities/compressor/types/zlibcompressor.hpp>

using namespace love;

Compressor* Compressor::GetCompressor(Format format)
{
    static LZ4Compressor lz4Compressor;
    static ZlibCompressor zlibCompressor;

    Compressor* compressors[] = { &lz4Compressor, &zlibCompressor };

    for (Compressor* compressor : compressors)
    {
        if (compressor->IsSupported(format))
            return compressor;
    }

    return nullptr;
}
