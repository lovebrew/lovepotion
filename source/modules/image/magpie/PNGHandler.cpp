#include "common/Exception.hpp"

#include "modules/image/magpie/PNGHandler.hpp"

#include <libpng16/png.h>

#include <algorithm>

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

        image.format = PNG_FORMAT_RGBA;

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
        image.format  = PNG_FORMAT_RGBA;

        png_image_write_get_memory_size(image, result.size, 1, decoded.data, PNG_IMAGE_ROW_STRIDE(image),
                                        NULL);

        if (PNG_IMAGE_FAILED(image))
        {
            png_image_free(&image);
            throw love::Exception("Failed to write PNG image: {:s}", image.message);
        }

        result.data = new uint8_t[result.size];
        png_image_write_to_memory(&image, result.data, &result.size, 1, decoded.data,
                                  PNG_IMAGE_ROW_STRIDE(image), NULL);

        if (PNG_IMAGE_FAILED(image))
        {
            png_image_free(&image);
            throw love::Exception("Failed to write PNG image: {:s}", image.message);
        }

        return result;
    }
} // namespace love
