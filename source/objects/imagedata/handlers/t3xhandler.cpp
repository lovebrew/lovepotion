#include "objects/imagedata/handlers/t3xhandler.h"

#include "common/exception.h"
#include "common/lmath.h"

using namespace love;

bool T3XHandler::CanDecode(Data* data)
{
    Tex3DSHeader header {};
    memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        return false;
    else if (header.type != GPU_TEX_2D)
        return false;
    else if (header.format = GPU_RGBA8)
        return false;
    else if (header.mipmapLevels != 0)
        return false;
    else if (header.compressionType != 0)
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

    DecodedImage decoded {};

    decoded.width  = header.width;
    decoded.height = header.height;
    decoded.format = PIXELFORMAT_RGBA8;
    decoded.size   = header.size;

    decoded.data = new uint8_t[decoded.size];

    memcpy(decoded.data, data->GetData() + sizeof(header), header.size);

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
    header.width_log2  = log2(decoded.width) - 3;
    header.height_log2 = log2(decoded.height) - 3;

    header.type         = GPU_TEX_2D;
    header.format       = GPU_RGBA8;
    header.mipmapLevels = 0;

    // source texture size
    header.width  = decoded.width;
    header.height = decoded.height;

    unsigned border = 1;

    header.left   = border;
    header.top    = decoded.height - border;
    header.right  = decoded.width + border;
    header.bottom = decoded.height - border - header.height;

    header.size = 4 * decoded.width * decoded.height;

    header.compressionType = 0;

    EncodedImage encoded {};

    encoded.data = new uint8_t[sizeof(header) + header.size];

    memcpy(encoded.data, &header, sizeof(header));
    memcpy(encoded.data + sizeof(header), decoded.data + sizeof(header), header.size);
}

void T3XHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
