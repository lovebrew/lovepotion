#include "modules/data/compressor/compressor.h"

#include "common/bidirectionalmap.h"
#include "modules/data/compressor/types/lz4compressor.h"
#include "modules/data/compressor/types/zlibcompressor.h"

using namespace love;

Compressor* Compressor::GetCompressor(Format format)
{
    static LZ4Compressor lz4Compressor;
    static zlibCompressor zlibCompressor;

    Compressor* compressors[] = { &lz4Compressor, &zlibCompressor };

    for (Compressor* compressor : compressors)
    {
        if (compressor->IsSupported(format))
            return compressor;
    }

    return nullptr;
}

// clang-format off
constexpr auto formatNames = BidirectionalMap<>::Create(
    "lz4",     Compressor::Format::FORMAT_LZ4,
    "zlib",    Compressor::Format::FORMAT_ZLIB,
    "gzip",    Compressor::Format::FORMAT_GZIP,
    "deflate", Compressor::Format::FORMAT_DEFLATE
);
// clang-format on

bool Compressor::GetConstant(const char* in, Format& out)
{
    return formatNames.Find(in, out);
}

bool Compressor::GetConstant(Format in, const char*& out)
{
    return formatNames.ReverseFind(in, out);
}

std::vector<const char*> Compressor::GetConstants(Format)
{
    return formatNames.GetNames();
}
