#include <common/exception.hpp>

#include <utilities/formathandler/types/ddshandler.hpp>

#include <objects/imagedata_ext.hpp>

#include <objects/compressedimagedata/compressedslice.hpp>

#include <ddsparse.h>

using namespace love;
using namespace dds::dxinfo;
using CompressedSlices = std::vector<StrongReference<CompressedSlice>>;

static PixelFormat convertFormat(DXGIFormat format, bool& isSRGB, bool& isBRGA)
{
    isSRGB = false;
    isBRGA = false;

    switch (format)
    {
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return PIXELFORMAT_RGBA16_UNORM;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            isSRGB = (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
            return PIXELFORMAT_RGBA8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            isSRGB = (format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
            isBRGA = true;
            return PIXELFORMAT_RGBA8_UNORM;
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            isSRGB = (format == DXGI_FORMAT_BC1_UNORM_SRGB);
            return PIXELFORMAT_DXT1_UNORM;
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            isSRGB = (format == DXGI_FORMAT_BC2_UNORM_SRGB);
            return PIXELFORMAT_DXT3_UNORM;
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            isSRGB = (format == DXGI_FORMAT_BC3_UNORM_SRGB);
            return PIXELFORMAT_DXT5_UNORM;
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
            return PIXELFORMAT_BC4_UNORM;
        case DXGI_FORMAT_BC4_SNORM:
            return PIXELFORMAT_BC4_SNORM;
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
            return PIXELFORMAT_BC5_UNORM;
        case DXGI_FORMAT_BC5_SNORM:
            return PIXELFORMAT_BC5_SNORM;
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            isSRGB = (format == DXGI_FORMAT_BC7_UNORM_SRGB);
            return PIXELFORMAT_BC7_UNORM;
        case DXGI_FORMAT_B5G6R5_UNORM:
            return PIXELFORMAT_RGB565_UNORM;
        default:
            return PIXELFORMAT_UNKNOWN;
    }
}

bool DDSHandler::CanDecode(Data* data)
{
    DXGIFormat ddsFormat = dds::getDDSPixelFormat(data->GetData(), data->GetSize());
    bool isSRGB          = false;
    bool isBRGA          = false;

    PixelFormat format = convertFormat(ddsFormat, isSRGB, isBRGA);
    return ImageData<Console::Which>::ValidPixelFormat(format);
}

FormatHandler::DecodedImage DDSHandler::Decode(Data* data)
{
    DecodedImage image {};
    dds::Parser parser(data->GetData(), data->GetSize());

    bool isSRGB = false;
    bool isBRGA = false;

    image.format = convertFormat(parser.getFormat(), isSRGB, isBRGA);

    if (!ImageData<Console::Which>::ValidPixelFormat(image.format))
        throw love::Exception("Could not parse DDS pixel data: Unsupported format.");

    if (parser.getMipmapCount() == 0)
        throw love::Exception("Could not parse DDS pixel data: No readable texture data.");

    const dds::Image* ddsImage = parser.getImageData(0);
    image.data                 = std::make_unique<uint8_t[]>(ddsImage->dataSize);

    std::memcpy(image.data.get(), ddsImage->data, ddsImage->dataSize);

    image.size   = ddsImage->dataSize;
    image.width  = ddsImage->width;
    image.height = ddsImage->height;

    if (isBRGA)
    {
        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                size_t offset = (y * image.width + x) * 4;
                std::swap(image.data[offset + 0], image.data[offset + 2]);
            }
        }
    }

    return image;
}

bool DDSHandler::CanParseCompressed(Data* data)
{
    return dds::isCompressedDDS(data->GetData(), data->GetSize());
}

StrongReference<ByteData> DDSHandler::ParseCompressed(Data* data, CompressedSlices& images,
                                                      PixelFormat& format, bool& srgb)
{
    if (!dds::isCompressedDDS(data->GetData(), data->GetSize()))
        throw love::Exception("Could not decode compressed data (not a DDS file?)");

    PixelFormat textureFormat = PIXELFORMAT_UNKNOWN;

    bool isSRGB = false;
    bool isBRGA = false;

    size_t dataSize = 0;
    images.clear();

    dds::Parser parser(data->GetData(), data->GetSize());
    textureFormat = convertFormat(parser.getFormat(), isSRGB, isBRGA);

    if (textureFormat == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse compressed data: Unsupported format.");

    if (parser.getMipmapCount() == 0)
        throw love::Exception("Could not parse compressed data: No readable texture data.");

    for (size_t index = 0; index < parser.getMipmapCount(); index++)
    {
        const auto* image = parser.getImageData(index);
        dataSize += image->dataSize;
    }

    StrongReference<ByteData> memory(new ByteData(dataSize, false), Acquire::NORETAIN);
    size_t dataOffset = 0;

    for (size_t index = 0; index < parser.getMipmapCount(); index++)
    {
        const auto* image = parser.getImageData(index);
        std::memcpy((uint8_t*)memory->GetData() + dataOffset, image->data, image->dataSize);

        auto slice = new CompressedSlice(textureFormat, image->width, image->height, memory,
                                         dataOffset, image->dataSize);

        images.emplace_back(slice, Acquire::NORETAIN);
        dataOffset += image->dataSize;
    }

    format = textureFormat;
    srgb   = isSRGB;

    return memory;
}