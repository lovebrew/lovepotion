#include "objects/compressedimagedata/handlers/ddshandler.h"
#include "common/exception.h"

#include "ddsparse/ddsparse.h"
#include "objects/imagedata/imagedata.h"

using namespace love;
using namespace dds::dxinfo;

static PixelFormat convertFormat(DXGIFormat dxFormat, bool& isSRGB, bool& isBGRA)
{
    isSRGB = false;
    isBGRA = false;

    switch (dxFormat)
    {
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return PIXELFORMAT_RGBA16;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            isSRGB = (dxFormat == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
            return PIXELFORMAT_RGBA8;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            isSRGB = (dxFormat == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
            isBGRA = true;
            return PIXELFORMAT_RGBA8;
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            isSRGB = (dxFormat == DXGI_FORMAT_BC1_UNORM_SRGB);
            return PIXELFORMAT_DXT1;
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            isSRGB = (dxFormat == DXGI_FORMAT_BC2_UNORM_SRGB);
            return PIXELFORMAT_DXT3;
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            isSRGB = (dxFormat == DXGI_FORMAT_BC3_UNORM_SRGB);
            return PIXELFORMAT_DXT5;
        default:
            return PIXELFORMAT_UNKNOWN;
    }
}

bool DDSHandler::CanDecode(Data* data)
{
    DXGIFormat dxFormat = dds::getDDSPixelFormat(data->GetData(), data->GetSize());

    bool isSRGB = false;
    bool isBGRA = false;

    PixelFormat format = convertFormat(dxFormat, isSRGB, isBGRA);

    return ImageData::ValidatePixelFormat(format);
}

FormatHandler::DecodedImage DDSHandler::Decode(Data* data)
{
    DecodedImage decoded {};
    dds::Parser parser(data->GetData(), data->GetSize());

    bool isSRGB = false;
    bool isBGRA = false;

    decoded.format = convertFormat(parser.getFormat(), isSRGB, isBGRA);

    if (!ImageData::ValidatePixelFormat(decoded.format))
        throw love::Exception("Could not parse DDS pixel data: Unsupported format.");

    if (parser.getMipmapCount() == 0)
        throw love::Exception("Could not parse DDS pixel data: No readable texture data.");

    const dds::Image* image = parser.getImageData(0);

    try
    {
        decoded.data = new uint8_t[image->dataSize];
    }
    catch (std::exception&)
    {
        throw love::Exception("Out of memory.");
    }

    memcpy(decoded.data, image->data, image->dataSize);

    decoded.size   = image->dataSize;
    decoded.width  = image->width;
    decoded.height = image->height;

    // Swap red and blue channels for incoming BGRA data.
    if (isBGRA)
    {
        for (int y = 0; y < decoded.height; y++)
        {
            for (int x = 0; x < decoded.width; x++)
            {
                size_t offset = (y * decoded.width + x) * 4;

                uint8_t b = decoded.data[offset + 0];
                uint8_t r = decoded.data[offset + 2];

                decoded.data[offset + 0] = r;
                decoded.data[offset + 2] = b;
            }
        }
    }

    return decoded;
}

bool DDSHandler::CanParseCompressed(Data* data)
{
    return dds::isCompressedDDS(data->GetData(), data->GetSize());
}

StrongReference<CompressedMemory> DDSHandler::ParseCompressed(
    Data* fileData, std::vector<StrongReference<CompressedSlice>>& images, PixelFormat& format,
    bool& sRGB)
{
    if (!dds::isCompressedDDS(fileData->GetData(), fileData->GetSize()))
        throw love::Exception("Could not decode compressed data (not a *.dds file?)");

    PixelFormat pixelFormat = PIXELFORMAT_UNKNOWN;

    bool isSRGB = false;
    bool isBGRA = false;

    StrongReference<CompressedMemory> memory;
    size_t dataSize = 0;

    images.clear();

    dds::Parser parser(fileData->GetData(), fileData->GetSize());
    pixelFormat = convertFormat(parser.getFormat(), isSRGB, isBGRA);

    if (pixelFormat == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse compressed data: Unsupported format.");

    if (parser.getMipmapCount() == 0)
        throw love::Exception("Could not parse compressed data: No readable texture data.");

    for (size_t index = 0; index < parser.getMipmapCount(); index++)
    {
        const dds::Image* image = parser.getImageData(index);
        dataSize += image->dataSize;
    }

    memory.Set(new CompressedMemory(dataSize), Acquire::NORETAIN);
    size_t dataOffset = 0;

    for (size_t index = 0; index < parser.getMipmapCount(); index++)
    {
        const dds::Image* image = parser.getImageData(index);
        memcpy(memory->data + dataOffset, image->data, image->dataSize);

        auto slice = new CompressedSlice(pixelFormat, image->width, image->height, memory,
                                         dataOffset, image->dataSize);
        images.emplace_back(slice, Acquire::NORETAIN);

        dataOffset += image->dataSize;
    }

    format = pixelFormat;
    sRGB   = isSRGB;

    return memory;
}
