#include "common/Exception.hpp"
#include "driver/display/citro3d.hpp"

#include "modules/image/magpie/T3XHandler.hpp"

#include <3ds.h>

#include <algorithm>
#include <cstring>

namespace love
{
    struct __attribute__((packed)) Tex3DSHeader
    {
        uint16_t numSubTextures; //< 1

        uint8_t width_log2 : 3;  //< log2(texWidth)  - 3 (powTwoWidth)
        uint8_t height_log2 : 3; //< log2(texHeight) - 3 (powTwoHeight)

        uint8_t type : 1;     //< 0 = GPU_TEX_2D
        uint8_t format;       //< 0 = GPU_RGBA8
        uint8_t mipmapLevels; //< 0

        uint16_t width;  //< subtexWidth  (sourceWidth)
        uint16_t height; //< subtexHeight (sourceHeight)

        uint16_t left;   //< left   = border
        uint16_t top;    //< top    = texHeight
        uint16_t right;  //< right  = subtexWidth
        uint16_t bottom; //< bottom = texHeight - subtexHeight
    };

    static bool isValidTex3DSTexture(Data* data, PixelFormat& format)
    {
        Tex3DSHeader header {};
        std::memcpy(&header, data->getData(), sizeof(Tex3DSHeader));

        if (header.numSubTextures != 1)
            return false;
        else if (header.type != GPU_TEX_2D)
            return false;

        if (!citro3d::getConstant((GPU_TEXCOLOR)header.format, format))
            return false;

        return true;
    }

    static void validateDimensions(const size_t width, const size_t height)
    {
        if (width > LOVE_TEX3DS_MAX || height > LOVE_TEX3DS_MAX)
            throw love::Exception("Texture dimensions exceed 3DS limits.");
        else if (width < LOVE_TEX3DS_MIN || height < LOVE_TEX3DS_MIN)
            throw love::Exception("Texture dimensions are too small.");
    }

    bool T3XHandler::canDecode(Data* data) const
    {
        auto format = PIXELFORMAT_MAX_ENUM;
        if (!isValidTex3DSTexture(data, format))
            return false;

        return !love::isPixelFormatCompressed(format);
    }

    FormatHandler::DecodedImage T3XHandler::decode(Data* data) const
    {
        Tex3DSHeader header {};
        std::memcpy(&header, data->getData(), sizeof(Tex3DSHeader));

        PixelFormat result = PIXELFORMAT_MAX_ENUM;
        const auto format  = (GPU_TEXCOLOR)header.format;

        citro3d::getConstant(format, result);

        const size_t width  = 1 << (header.width_log2 + 3);
        const size_t height = 1 << (header.height_log2 + 3);

        validateDimensions(width, height);

        DecodedImage image {};
        image.width  = header.width;
        image.height = header.height;
        image.format = result;

        const auto size       = data->getSize() - sizeof(Tex3DSHeader);
        const auto compressed = (uint8_t*)data->getData() + sizeof(Tex3DSHeader);

        size_t uncompressedSize = 0;
        if (decompressHeader(nullptr, &uncompressedSize, nullptr, compressed, size) < 0)
            throw love::Exception("Failed to decompress texture header.");

        image.size = (header.width * header.height * love::getPixelFormatBlockSize(result));

        try
        {
            image.data = new uint8_t[uncompressedSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        if (!decompress(image.data, uncompressedSize, nullptr, compressed, size))
        {
            delete[] image.data;
            throw love::Exception("Failed to decompress texture.");
        }

        return image;
    }

    bool T3XHandler::canParseCompressed(Data* data) const
    {
        auto format = PIXELFORMAT_MAX_ENUM;
        if (!isValidTex3DSTexture(data, format))
            return false;

        return love::isPixelFormatCompressed(format);
    }

    StrongRef<ByteData> T3XHandler::parseCompressed(Data* filedata, CompressedSlices& images,
                                                    PixelFormat& format) const
    {
        Tex3DSHeader header {};
        std::memcpy(&header, filedata->getData(), sizeof(Tex3DSHeader));

        auto convertedFormat = PIXELFORMAT_MAX_ENUM;
        if (!citro3d::getConstant((GPU_TEXCOLOR)header.format, convertedFormat))
            throw love::Exception("Invalid texture format.");

        const size_t width  = 1 << (header.width_log2 + 3);
        const size_t height = 1 << (header.height_log2 + 3);

        validateDimensions(width, height);

        const auto size       = filedata->getSize() - sizeof(Tex3DSHeader);
        const auto compressed = (uint8_t*)filedata->getData() + sizeof(Tex3DSHeader);

        size_t uncompressedSize = 0;
        if (decompressHeader(nullptr, &uncompressedSize, nullptr, compressed, size) < 0)
            throw love::Exception("Failed to decompress texture header.");

        uint8_t* data = nullptr;

        try
        {
            data = new uint8_t[uncompressedSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        if (!decompress(data, uncompressedSize, nullptr, compressed, size))
        {
            delete[] data;
            throw love::Exception("Failed to decompress texture.");
        }

        StrongRef<ByteData> memory(new ByteData(uncompressedSize, false), Acquire::NO_RETAIN);
        std::memcpy(memory->getData(), data, uncompressedSize);

        // clang-format off
        images.emplace_back(new CompressedSlice(convertedFormat, header.width, header.height, memory, 0, uncompressedSize), Acquire::NO_RETAIN);
        // clang-format on

        format = convertedFormat;
        return memory;
    }
} // namespace love
