#include "common/Exception.hpp"
#include "common/Color.hpp"
#include "common/math.hpp"

#include "modules/image/magpie/PNGHandler.hpp"

#if defined(__3DS__)
    #include "modules/image/magpie/TileImage.hpp"
#endif

#include <cstddef>
#include <libpng16/png.h>

#include <algorithm>
#include <memory>

namespace love
{
    bool PNGHandler::canDecode(Data* data) const
    {
        png_image image {};
        image.version = PNG_IMAGE_VERSION;

        png_image_begin_read_from_memory(&image, data->getData(), data->getSize());

        if (PNG_IMAGE_FAILED(image))
        {
            png_image_free(&image);
            return false;
        }

        png_image_free(&image);
        return true;
    }

    FormatHandler::DecodedImage PNGHandler::decode(Data* data) const
    {
        png_image image {};
        image.version = PNG_IMAGE_VERSION;

        png_image_begin_read_from_memory(&image, data->getData(), data->getSize());

        if (PNG_IMAGE_FAILED(image))
        {
            png_image_free(&image);
            throw love::Exception("Failed to read PNG image: {:s}", image.message);
        }

#if defined(__3DS__)
        image.format = PNG_FORMAT_ABGR; // Why.
#else
        image.format = PNG_FORMAT_RGBA;
#endif

        DecodedImage result {};
        result.width  = image.width;
        result.height = image.height;
        result.format = PIXELFORMAT_RGBA8_UNORM;
        result.size   = (image.width * image.height) * sizeof(uint32_t);
        result.data   = new uint8_t[result.size];

        png_image_finish_read(&image, nullptr, result.data, PNG_IMAGE_ROW_STRIDE(image), nullptr);

        if (PNG_IMAGE_FAILED(image))
        {
            png_image_free(&image);
            throw love::Exception("Failed to read PNG image: {:s}", image.message);
        }

        png_image_free(&image);

#if defined(__3DS__)
        tileImage(result);
#endif

        return result;
    }

    bool PNGHandler::canEncode(PixelFormat rawFormat, EncodedFormat encodedFormat) const
    {
        return encodedFormat == ENCODED_PNG &&
               (rawFormat == PIXELFORMAT_RGBA8_UNORM || rawFormat == PIXELFORMAT_RGBA16_UNORM);
    }

    FormatHandler::EncodedImage PNGHandler::encode(const DecodedImage& decoded,
                                                   EncodedFormat encodedFormat) const
    {
        if (!this->canEncode(decoded.format, encodedFormat))
            throw love::Exception("PNG encoder cannot encode to non-PNG format.");

        EncodedImage result {};

        png_image image {};
        image.width   = decoded.width;
        image.height  = decoded.height;
        image.version = PNG_IMAGE_VERSION;
#if defined(__3DS__)
        image.format = PNG_FORMAT_ABGR;

        auto linearData = std::make_unique<uint32_t[]>(decoded.width * decoded.height);
        for (int y = 0; y < decoded.height; y++)
        {
            for (int x = 0; x < decoded.width; x++)
            {
                const auto* pixel = colorFromTile<uint32_t>(decoded.data, NextPo2(decoded.width), x, y);
                linearData[y * decoded.width + x] = *pixel;
            }
        }

        const void* pixelData = linearData.get();
#else
        image.format  = PNG_FORMAT_RGBA;
        const void* pixelData = decoded.data;
#endif

        if (!png_image_write_get_memory_size(image, result.size, 1, pixelData,
                                             PNG_IMAGE_ROW_STRIDE(image), NULL))
        {
            png_image_free(&image);
            throw love::Exception("Failed to write PNG image: {:s}", image.message);
        }

        result.data = new uint8_t[result.size];
        png_image_write_to_memory(&image, result.data, &result.size, 1, pixelData,
                                  PNG_IMAGE_ROW_STRIDE(image), NULL);

        if (PNG_IMAGE_FAILED(image))
        {
            png_image_free(&image);
            delete[] result.data;
            throw love::Exception("Failed to write PNG image: {:s}", image.message);
        }

        png_image_free(&image);
        return result;
    }
} // namespace love
