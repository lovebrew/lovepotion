#include <utilities/formathandler/types/pnghandler.hpp>

#include <common/exception.hpp>

#include <algorithm>

#include <libpng16/png.h>

using namespace love;

bool PNGHandler::CanDecode(Data* data)
{
    png_image image {};
    image.version = PNG_IMAGE_VERSION;

    png_image_begin_read_from_memory(&image, data->GetData(), data->GetSize());

    if (PNG_IMAGE_FAILED(image))
    {
        png_image_free(&image);
        return false;
    }

    png_image_free(&image);
    return true;
}

PNGHandler::DecodedImage PNGHandler::Decode(Data* data)
{
    DecodedImage decoded {};

    png_image image {};
    image.version = PNG_IMAGE_VERSION;

    png_image_begin_read_from_memory(&image, data->GetData(), data->GetSize());

    if (PNG_IMAGE_FAILED(image))
    {
        png_image_free(&image);
        throw love::Exception("Could not decode PNG image: %s", image.message);
    }

    image.format = PNG_FORMAT_RGBA;

    decoded.width  = image.width;
    decoded.height = image.height;
    decoded.format = PIXELFORMAT_RGBA8_UNORM;
    decoded.size   = (image.width * image.height) * sizeof(uint32_t);

    decoded.data = std::make_unique<uint8_t[]>(decoded.size);

    png_image_finish_read(&image, nullptr, decoded.data.get(), PNG_IMAGE_ROW_STRIDE(image),
                          nullptr);

    if (PNG_IMAGE_FAILED(image))
    {
        png_image_free(&image);
        throw love::Exception("Could not decode PNG image: %s", image.message);
    }

    png_image_free(&image);

    return decoded;
}

bool PNGHandler::CanEncode(PixelFormat format, EncodedFormat encodedFormat)
{
    bool isRGBA8  = (format == PIXELFORMAT_RGBA8_UNORM);
    bool isRGBA16 = (format == PIXELFORMAT_RGBA16_UNORM);

    return encodedFormat == ENCODED_PNG && (isRGBA8 || isRGBA16);
}

PNGHandler::EncodedImage PNGHandler::Encode(const DecodedImage& decoded, EncodedFormat format)
{
    if (!this->CanEncode(decoded.format, format))
        throw love::Exception("PNG encoder cannot encode to non-PNG format.");

    EncodedImage encoded {};

    png_image image {};

    image.width   = decoded.width;
    image.height  = decoded.height;
    image.version = PNG_IMAGE_VERSION;
    image.format  = PNG_FORMAT_RGBA;

    bool success = png_image_write_get_memory_size(image, encoded.size, 1, decoded.data.get(),
                                                   PNG_IMAGE_ROW_STRIDE(image), NULL);

    if (!success)
        throw love::Exception("Failed to get memory size for PNG.");

    encoded.data = std::make_unique<uint8_t[]>(encoded.size);

    success = png_image_write_to_memory(&image, encoded.data.get(), &encoded.size, 1,
                                        decoded.data.get(), PNG_IMAGE_ROW_STRIDE(image), NULL);

    if (!success)
        throw love::Exception("An error occurred on png_image_write_to_memory!");

    return encoded;
}
