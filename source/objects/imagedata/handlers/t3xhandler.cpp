#include "objects/imagedata/handlers/t3xhandler.h"

#include "common/exception.h"
#include "common/lmath.h"

#include "common/colors.h"

#include "modules/data/datamodule.h"
#include <memory>

using namespace love;

#if defined(__3DS__)
bool T3XHandler::CanDecode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        return false;
    else if (header.type != 0) //< GPU_TEX_2D
        return false;
    else if (header.format != 0) //< GPU_RGBA8
        return false;
    else if (header.mipmapLevels != 0)
        return false;

    return true;
}

T3XHandler::DecodedImage T3XHandler::Decode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    DecodedImage decoded {};

    decoded.width  = 1 << (header.width_log2 + 3);
    decoded.height = 1 << (header.height_log2 + 3);
    decoded.format = PIXELFORMAT_RGBA8;
    decoded.size   = 4 * header.width * header.height;
    decoded.data   = new uint8_t[header.size];

    if (!decompress(decoded.data, header.size, NULL,
                    reinterpret_cast<uint8_t*>(data->GetData()) + sizeof(Tex3DSHeader) - 4,
                    data->GetSize() - sizeof(Tex3DSHeader) - 4))
    {
        throw love::Exception("Failed to decompress t3x data.");
    }

    return decoded;
}
#endif

void T3XHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
