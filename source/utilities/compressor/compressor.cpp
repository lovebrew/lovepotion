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

// clang-format off
constexpr BidirectionalMap formatNames = {
    "lz4",     Compressor::Format::FORMAT_LZ4,
    "zlib",    Compressor::Format::FORMAT_ZLIB,
    "gzip",    Compressor::Format::FORMAT_GZIP,
    "deflate", Compressor::Format::FORMAT_DEFLATE
};
// clang-format on

bool Compressor::GetConstant(const char* in, Format& out)
{
    return formatNames.Find(in, out);
}

bool Compressor::GetConstant(Format in, const char*& out)
{
    return formatNames.ReverseFind(in, out);
}

SmallTrivialVector<const char*, 4> Compressor::GetConstants(Format)
{
    return formatNames.GetNames();
}
