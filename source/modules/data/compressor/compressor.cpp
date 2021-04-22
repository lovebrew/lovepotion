#include "modules/data/compressor/compressor.h"

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

bool Compressor::GetConstant(const char* in, Format& out)
{
    return formatNames.Find(in, out);
}

bool Compressor::GetConstant(Format in, const char*& out)
{
    return formatNames.Find(in, out);
}

std::vector<const char*> Compressor::GetConstants(Format)
{
    return formatNames.GetNames();
}

// clang-format off
constexpr StringMap<Compressor::Format, Compressor::FORMAT_MAX_ENUM>::Entry formatEntries[] =
{
    { "lz4",     Compressor::Format::FORMAT_LZ4     },
    { "zlib",    Compressor::Format::FORMAT_ZLIB    },
    { "gzip",    Compressor::Format::FORMAT_GZIP    },
    { "deflate", Compressor::Format::FORMAT_DEFLATE }
};

constinit const StringMap<Compressor::Format, Compressor::FORMAT_MAX_ENUM> Compressor::formatNames(formatEntries);
// clang-format on
