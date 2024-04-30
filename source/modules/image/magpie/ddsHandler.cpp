#include "common/Exception.hpp"

#include "modules/image/ImageData.hpp"
#include "modules/image/magpie/ddsHandler.hpp"

#include <ddsparse.h>
using namespace dds::dxinfo;

#include <cstring>

#define E_COULD_NOT_PARSE "Could not parse compressed data: {:s}"

namespace love
{
    static PixelFormat convertFormat(dds::dxinfo::DXGIFormat format)
    {
        switch (format)
        {
            case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                return PIXELFORMAT_RGBA32_FLOAT;
            case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return PIXELFORMAT_RGBA16_FLOAT;
            case DXGI_FORMAT_R16G16B16A16_UNORM:
                return PIXELFORMAT_RGBA16_UNORM;
            case DXGI_FORMAT_R32G32_TYPELESS:
            case DXGI_FORMAT_R32G32_FLOAT:
                return PIXELFORMAT_RG32_FLOAT;
            case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            case DXGI_FORMAT_R10G10B10A2_UNORM:
                return PIXELFORMAT_RGB10A2_UNORM;
            case DXGI_FORMAT_R11G11B10_FLOAT:
                return PIXELFORMAT_RG11B10_FLOAT;
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                return PIXELFORMAT_RGBA8_UNORM;
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return PIXELFORMAT_RGBA8_sRGB;
            case DXGI_FORMAT_R16G16_TYPELESS:
            case DXGI_FORMAT_R16G16_FLOAT:
                return PIXELFORMAT_RG16_FLOAT;
            case DXGI_FORMAT_R16G16_UNORM:
                return PIXELFORMAT_RG16_UNORM;
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_R32_FLOAT:
                return PIXELFORMAT_R32_FLOAT;
            case DXGI_FORMAT_R8G8_TYPELESS:
            case DXGI_FORMAT_R8G8_UNORM:
                return PIXELFORMAT_RG8_UNORM;
            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_R16_FLOAT:
                return PIXELFORMAT_R16_FLOAT;
            case DXGI_FORMAT_R16_UNORM:
                return PIXELFORMAT_R16_UNORM;
            case DXGI_FORMAT_R8_TYPELESS:
            case DXGI_FORMAT_R8_UNORM:
            case DXGI_FORMAT_A8_UNORM:
                return PIXELFORMAT_R8_UNORM;
            case DXGI_FORMAT_BC1_TYPELESS:
            case DXGI_FORMAT_BC1_UNORM:
                return PIXELFORMAT_DXT1_UNORM;
            case DXGI_FORMAT_BC1_UNORM_SRGB:
                return PIXELFORMAT_DXT1_sRGB;
            case DXGI_FORMAT_BC2_TYPELESS:
            case DXGI_FORMAT_BC2_UNORM:
                return PIXELFORMAT_DXT3_UNORM;
            case DXGI_FORMAT_BC2_UNORM_SRGB:
                return PIXELFORMAT_DXT3_sRGB;
            case DXGI_FORMAT_BC3_TYPELESS:
            case DXGI_FORMAT_BC3_UNORM:
                return PIXELFORMAT_DXT5_UNORM;
            case DXGI_FORMAT_BC3_UNORM_SRGB:
                return PIXELFORMAT_DXT5_sRGB;
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
            case DXGI_FORMAT_B5G6R5_UNORM:
                return PIXELFORMAT_RGB565_UNORM;
            case DXGI_FORMAT_B5G5R5A1_UNORM:
                return PIXELFORMAT_RGB5A1_UNORM;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                return PIXELFORMAT_BGRA8_UNORM;
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                return PIXELFORMAT_BGRA8_sRGB;
            case DXGI_FORMAT_BC6H_TYPELESS:
            case DXGI_FORMAT_BC6H_UF16:
                return PIXELFORMAT_BC6H_UFLOAT;
            case DXGI_FORMAT_BC6H_SF16:
                return PIXELFORMAT_BC6H_FLOAT;
            case DXGI_FORMAT_BC7_TYPELESS:
            case DXGI_FORMAT_BC7_UNORM:
                return PIXELFORMAT_BC7_UNORM;
            case DXGI_FORMAT_BC7_UNORM_SRGB:
                return PIXELFORMAT_BC7_sRGB;
            default:
                return PIXELFORMAT_UNKNOWN;
        }
    }

    bool DDSHandler::canDecode(Data* data) const
    {
        DXGIFormat dxFormat = dds::getDDSPixelFormat(data->getData(), data->getSize());
        PixelFormat format  = convertFormat(dxFormat);

        if (format == PIXELFORMAT_BGRA8_UNORM)
            format = PIXELFORMAT_RGBA8_UNORM;
        else if (format == PIXELFORMAT_BGRA8_sRGB)
            format = PIXELFORMAT_RGBA8_sRGB;

        return ImageData::validPixelFormat(format);
    }

    FormatHandler::DecodedImage DDSHandler::decode(Data* data) const
    {
        dds::Parser parser(data->getData(), data->getSize());

        DecodedImage image {};
        image.format = convertFormat(parser.getFormat());

        bool bgra = false;

        if (image.format == PIXELFORMAT_BGRA8_UNORM)
        {
            image.format = PIXELFORMAT_RGBA8_UNORM;
            bgra         = true;
        }
        else if (image.format == PIXELFORMAT_BGRA8_sRGB)
        {
            image.format = PIXELFORMAT_RGBA8_sRGB;
            bgra         = true;
        }

        if (!ImageData::validPixelFormat(image.format))
            throw love::Exception("Could not parse DDS pixel data: Unsupported format.");

        if (parser.getMipmapCount() == 0)
            throw love::Exception("Could not parse DDS pixel data: No readable texture data.");

        const auto* ddsImage = parser.getImageData(0);

        try
        {
            image.data = new uint8_t[ddsImage->dataSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        std::memcpy(image.data, ddsImage->data, ddsImage->dataSize);

        image.size   = ddsImage->dataSize;
        image.width  = ddsImage->width;
        image.height = ddsImage->height;

        if (bgra)
        {
            for (int y = 0; y < image.height; y++)
            {
                for (int x = 0; x < image.width; x++)
                {
                    size_t offset = (y * image.width + x) * 4;

                    uint8_t b = image.data[offset + 0];
                    uint8_t r = image.data[offset + 2];

                    image.data[offset + 0] = r;
                    image.data[offset + 2] = b;
                }
            }
        }

        return image;
    }

    bool DDSHandler::canParseCompressed(Data* data) const
    {
        return dds::isCompressedDDS(data->getData(), data->getSize());
    }

    StrongRef<ByteData> DDSHandler::parseCompressed(Data* data, CompressedSlices& images,
                                                    PixelFormat& format) const
    {
        if (!dds::isCompressedDDS(data->getData(), data->getSize()))
            throw love::Exception("Could not decode compressed data (not a DDS file?)");

        auto textureFormat = PIXELFORMAT_UNKNOWN;
        size_t dataSize    = 0;

        images.clear();

        dds::Parser parser(data->getData(), data->getSize());
        textureFormat = convertFormat(parser.getFormat());

        if (textureFormat == PIXELFORMAT_UNKNOWN)
            throw love::Exception(E_COULD_NOT_PARSE, "unsupported format.");

        if (parser.getMipmapCount() == 0)
            throw love::Exception(E_COULD_NOT_PARSE, "no readable texture data.");

        for (size_t i = 0; i < parser.getMipmapCount(); i++)
        {
            const auto* image = parser.getImageData(i);
            dataSize += image->dataSize;
        }

        StrongRef<ByteData> memory(new ByteData(dataSize, false), Acquire::NO_RETAIN);
        size_t offset = 0;

        for (size_t i = 0; i < parser.getMipmapCount(); i++)
        {
            const auto* image = parser.getImageData(i);

            std::memcpy((uint8_t*)memory->getData() + offset, image->data, image->dataSize);

            // clang-format off
            auto* slice = new CompressedSlice(textureFormat, image->width, image->height, memory, offset, image->dataSize);
            // clang-format on

            images.emplace_back(slice, Acquire::NO_RETAIN);
            offset += image->dataSize;
        }

        format = textureFormat;
        return memory;
    }
} // namespace love
