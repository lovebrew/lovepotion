#include "objects/compressedimagedata/handlers/pkmhandler.h"
#include "common/exception.h"

using namespace love;

static inline uint16_t swapuint16(uint16_t in)
{
    return (in >> 8) | (in << 8);
}

static PixelFormat convertFormat(uint16_t format)
{
    switch (format)
    {
        case PKMHandler::PKMTextureFormat::ETC1_RGB_NO_MIPMAPS:
            return PIXELFORMAT_ETC1;
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGB_NO_MIPMAPS:
            return PIXELFORMAT_ETC2_RGB;
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD:
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGBA_NO_MIPMAPS:
            return PIXELFORMAT_ETC2_RGBA;
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGBA1_NO_MIPMAPS:
            return PIXELFORMAT_ETC2_RGBA1;
        default:
            return PIXELFORMAT_UNKNOWN;
    }
}

bool PKMHandler::CanParseCompressed(Data* data)
{
    if (data->GetSize() <= sizeof(PKMHeader))
        return false;

    const PKMHeader* header = (const PKMHeader*)data->GetData();

    if (memcmp(header->identifier, PKMHandler::PKMIDENTIFIER, 4) != 0)
        return false;

    if ((header->version[0] != '2' && header->version[0] != '1') || header->version[1] != '0')
        return false;

    return true;
}

StrongReference<CompressedMemory> PKMHandler::ParseCompressed(
    Data* fileData, std::vector<StrongReference<CompressedSlice>>& images, PixelFormat& format,
    bool& isSRGB)
{
    if (!this->CanParseCompressed(fileData))
        throw love::Exception("Could not decode compressed data (not a *.pkm file?)");

    PKMHeader header = *(const PKMHeader*)fileData->GetData();

    header.textureFormatBig = swapuint16(header.textureFormatBig);

    header.extendedWidthBig  = swapuint16(header.extendedWidthBig);
    header.extendedHeightBig = swapuint16(header.extendedHeightBig);

    header.widthBig  = swapuint16(header.widthBig);
    header.heightBig = swapuint16(header.heightBig);

    PixelFormat pixelFormat = convertFormat(header.textureFormatBig);

    if (pixelFormat == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse *.pkm file: unsupported texture format.");

    size_t totalSize = fileData->GetSize() - sizeof(PKMHeader);
    StrongReference<CompressedMemory> memory;

    memory.Set(new CompressedMemory(totalSize), Acquire::NORETAIN);
    memcpy(memory->data, (uint8_t*)fileData->GetData() + sizeof(PKMHeader), totalSize);

    int width  = header.widthBig;
    int height = header.heightBig;

    images.emplace_back(new CompressedSlice(pixelFormat, width, height, memory, 0, totalSize),
                        Acquire::NORETAIN);

    format = pixelFormat;
    isSRGB = false;

    return memory;
}
