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

std::vector<std::string> Compressor::GetConstants(Format)
{
    return formatNames.GetNames();
}

StringMap<Compressor::Format, Compressor::FORMAT_MAX_ENUM>::Entry Compressor::formatEntries[] = {
    { "lz4", FORMAT_LZ4 },
    { "zlib", FORMAT_ZLIB },
    { "gzip", FORMAT_GZIP },
    { "deflate", FORMAT_DEFLATE },
};

StringMap<Compressor::Format, Compressor::FORMAT_MAX_ENUM> Compressor::formatNames(
    Compressor::formatEntries, sizeof(Compressor::formatEntries));
