#include "objects/imagedata/handlers/t3xhandler.h"

#include "common/exception.h"
#include "common/lmath.h"

#include "common/colors.h"
#include "common/debug/logger.h"

using namespace love;

bool T3XHandler::CanDecode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        return false;
    else if (header.type != GPU_TEX_2D)
        return false;
    else if (header.format != GPU_RGBA8)
        return false;
    else if (header.mipmapLevels != 0)
        return false;

    return true;
}

bool T3XHandler::CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat)
{
    return encodedFormat == ENCODED_T3X;
}

T3XHandler::DecodedImage T3XHandler::Decode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    uint8_t* uncompressed = new uint8_t[header.size];
    decompress(uncompressed, header.size, NULL, (uint8_t*)data->GetData() + sizeof(header) - 4,
               data->GetSize() - sizeof(header) + 4);

    DecodedImage decoded {};

    decoded.width = header.width;
    LOG("%d", header.width);

    decoded.height = header.height;
    LOG("%d", header.height);

    decoded.format = PIXELFORMAT_RGBA8;

    unsigned powTwoWidth = NextPO2(decoded.width + 2);
    decoded.size         = 4 * header.width * header.height;
    LOG("%zu", decoded.size);

    decoded.data = new uint8_t[decoded.size];
    for (size_t y = 1; y <= header.height; y++)
    {
        for (size_t x = 1; x <= header.width; x++)
            memcpy((uint32_t*)decoded.data + ((x - 1) + (y - 1) * decoded.width),
                   (uint32_t*)uncompressed + coordToIndex(powTwoWidth, x, y), sizeof(uint32_t));
    }

    delete[] uncompressed;

    return decoded;
}

T3XHandler::EncodedImage T3XHandler::Encode(const DecodedImage& decoded,
                                            EncodedFormat encodedFormat)
{
    if (!this->CanEncode(decoded.format, encodedFormat))
        throw love::Exception("t3x encoder cannot encode to non-t3x format.");

    Tex3DSHeader header {};

    header.numSubTextures = 1;

    // power-of-two size
    header.width_log2  = log2(NextPO2(decoded.width + 2)) - 3;
    header.height_log2 = log2(NextPO2(decoded.height + 2)) - 3;

    header.type         = GPU_TEX_2D;
    header.format       = GPU_RGBA8;
    header.mipmapLevels = 0;

    // source texture size
    header.width  = decoded.width;
    header.height = decoded.height;

    unsigned border = 1;

    unsigned powTwoWidth  = NextPO2(decoded.width + 2);
    unsigned powTwoHeight = NextPO2(decoded.height + 2);

    header.left   = (border / (float)powTwoWidth) * 1024.0f;
    header.top    = (border / (float)powTwoHeight) * 1024.0f;
    header.right  = ((decoded.width + border) / (float)powTwoWidth) * 1024.0f;
    header.bottom = ((decoded.height + border) / (float)powTwoHeight) * 1024.0f;

    header.size = 4 * powTwoWidth * powTwoHeight;

    header.compressionType = 0;

    EncodedImage encoded {};

    size_t size = sizeof(header) + header.size;

    encoded.data = new uint8_t[size];
    encoded.size = size;

    memcpy(encoded.data, &header, sizeof(header));

    uint8_t* output = encoded.data + sizeof(header);
    memset(output, 0, header.size);

    for (unsigned y = 1; y <= (size_t)decoded.height; ++y)
    {
        for (unsigned x = 1; x <= (size_t)decoded.width; ++x)
        {
            const unsigned source_off = (y - 1) * decoded.width + (x - 1);
            const unsigned dest_off   = ((((y >> 3) * (powTwoWidth >> 3) + (x >> 3)) << 6) +
                                       ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
                                        ((x & 4) << 2) | ((y & 4) << 3)));

            memcpy((u32*)output + dest_off, (u32*)decoded.data + source_off, sizeof(u32));
        }
    }

    return encoded;
}

void T3XHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
