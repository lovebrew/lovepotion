#include <common/exception.hpp>

#include <objects/compressedimagedata/compressedslice.hpp>

#include <utilities/formathandler/types/pkmhandler.hpp>

#include <bit>

using namespace love;
using CompressedSlices = std::vector<StrongReference<CompressedSlice>>;

static inline uint16_t swap16big(uint16_t x)
{
    if (Console::IsBigEndian())
        return x;

    return (x >> 8) | (x << 8);
}

static PixelFormat convertFormat(uint16_t format)
{
    switch (format)
    {
        case PKMHandler::PKMTextureFormat::ETC1_RGB_NO_MIPMAPS:
            return PIXELFORMAT_ETC1_UNORM;
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGB_NO_MIPMAPS:
            return PIXELFORMAT_ETC2_RGB_UNORM;
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD:
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGBA_NO_MIPMAPS:
            return PIXELFORMAT_ETC2_RGBA_UNORM;
        case PKMHandler::PKMTextureFormat::ETC2PACKAGE_RGBA1_NO_MIPMAPS:
            return PIXELFORMAT_ETC2_RGBA1_UNORM;
        default:
            return PIXELFORMAT_UNKNOWN;
    }
}

bool PKMHandler::CanParseCompressed(Data* data)
{
    if (data->GetSize() <= sizeof(PKMHeader))
        return false;

    const auto* header = (const PKMHeader*)data->GetData();

    if (memcmp(header->identifier, identifier, 4) != 0)
        return false;

    if ((header->version[0] != '2' && header->version[0] != '1') || header->version[1] != '0')
        return false;

    return true;
}

StrongReference<ByteData> PKMHandler::ParseCompressed(Data* data, CompressedSlices& images,
                                                      PixelFormat& format, bool& sRGB)
{
    if (!this->CanParseCompressed(data))
        throw love::Exception("Could not decode compressed data (not a PKM file?)");

    auto header = *(const PKMHeader*)data->GetData();

    header.textureFormatBig  = swap16big(header.textureFormatBig);
    header.extendedWidthBig  = swap16big(header.extendedWidthBig);
    header.extendedHeightBig = swap16big(header.extendedHeightBig);
    header.widthBig          = swap16big(header.widthBig);
    header.heightBig         = swap16big(header.heightBig);

    PixelFormat textureFormat = convertFormat(header.textureFormatBig);

    if (textureFormat == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse PKM file: Unsupported texture format.");

    size_t totalSize = data->GetSize() - sizeof(PKMHeader);
    StrongReference<ByteData> memory(new ByteData(totalSize, false), Acquire::NORETAIN);

    std::memcpy(memory->GetData(), (uint8_t*)data->GetData() + sizeof(PKMHeader), totalSize);

    int width  = header.widthBig;
    int height = header.heightBig;

    images.emplace_back(new CompressedSlice(textureFormat, width, height, memory, 0, totalSize),
                        Acquire::NORETAIN);

    format = textureFormat;
    sRGB   = false;

    return memory;
}
