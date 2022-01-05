#include "objects/imagedata/handlers/t3xhandler.h"

#include "common/exception.h"
#include "common/lmath.h"

#include "common/colors.h"

#include "modules/data/datamodule.h"
#include <memory>

#include "debug/logger.h"

using namespace love;

#if defined(__3DS__)
bool T3XHandler::CanDecode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        return false;
    else if (header.type != GPU_TEX_2D) //< GPU_TEX_2D
        return false;
    else if (header.format != GPU_RGBA8) //< GPU_RGBA8
        return false;
    else if (header.mipmapLevels != 0)
        return false;

    return true;
}

T3XHandler::DecodedImage T3XHandler::Decode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        throw love::Exception("t3x does not contain exactly one subtexture");
    if (header.type != GPU_TEX_2D)
        throw love::Exception("t3x texture type is not GPU_TEX_2D");
    if (header.format != GPU_RGBA8)
        throw love::Exception("t3x GPU format is not GPU_RGBA8");

    auto const width  = 1 << (header.width_log2 + 3);
    auto const height = 1 << (header.height_log2 + 3);

    if (width > 1024 || height > 1024)
        throw love::Exception("t3x dimensions too large");

    DecodedImage decoded {};

    decoded.width  = width;
    decoded.height = height;
    decoded.format = PIXELFORMAT_TEX3DS_RGBA8;

    decoded.subWidth  = header.width;
    decoded.subHeight = header.height;

    auto const size       = data->GetSize() - sizeof(Tex3DSHeader);
    auto const compressed = reinterpret_cast<uint8_t*>(data->GetData()) + sizeof(Tex3DSHeader);

    size_t outSize = 0;
    if (decompressHeader(NULL, &outSize, NULL, compressed, size) == -1)
        throw love::Exception("Failed to decompress t3x header.");

    decoded.size = outSize;
    decoded.data = new uint8_t[outSize];

    if (!decompress(decoded.data, outSize, NULL, compressed, size))
        throw love::Exception("Failed to decompress t3x data.");

    return decoded;
}
#endif

void T3XHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
