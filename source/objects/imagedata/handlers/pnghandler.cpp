#include "objects/imagedata/handlers/pnghandler.h"

#include "common/exception.h"

#include "debug/logger.h"
#include <libpng16/png.h>

using namespace love;

bool PNGHandler::CanDecode(Data* data)
{
    png_image image;
    memset(&image, 0, sizeof(image));

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

bool PNGHandler::CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat)
{
    bool validFormat = rawFormat == PIXELFORMAT_RGBA8 || rawFormat == PIXELFORMAT_RGBA16;
    return encodedFormat == ENCODED_PNG && validFormat;
}

PNGHandler::DecodedImage PNGHandler::Decode(Data* data)
{
    DecodedImage decoded {};

    png_image image;
    memset(&image, 0, sizeof(image));

    image.version = PNG_IMAGE_VERSION;

    png_image_begin_read_from_memory(&image, data->GetData(), data->GetSize());

    if (PNG_IMAGE_FAILED(image))
    {
        png_image_free(&image);
        throw love::Exception("Could not decode PNG image (%s)", image.message);
    }

    image.format = PNG_FORMAT_RGBA;

    decoded.width  = image.width;
    decoded.height = image.height;
    decoded.format = PIXELFORMAT_RGBA8;
    decoded.size   = (image.width * image.height) * sizeof(uint32_t);

    decoded.data = new (std::align_val_t(4)) uint8_t[decoded.size];

    png_image_finish_read(&image, NULL, decoded.data, PNG_IMAGE_ROW_STRIDE(image), NULL);
    png_image_free(&image);

    if (PNG_IMAGE_FAILED(image))
        throw love::Exception("Could not decode PNG image (%s)", image.message);

    return decoded;
}

FormatHandler::EncodedImage PNGHandler::Encode(const DecodedImage& decoded,
                                               EncodedFormat encodedFormat)
{
    if (!this->CanEncode(decoded.format, encodedFormat))
        throw love::Exception("PNG encoder cannot encode to non-PNG format.");

    EncodedImage encoded {};

    png_image image;
    memset(&image, 0, sizeof(image));

    image.width   = decoded.width;
    image.height  = decoded.height;
    image.version = PNG_IMAGE_VERSION;
    image.format  = PNG_FORMAT_RGBA;

    png_image_write_get_memory_size(image, encoded.size, 1, decoded.data,
                                    PNG_IMAGE_ROW_STRIDE(image), NULL);

    encoded.data = new uint8_t[encoded.size];

    png_image_write_to_memory(&image, encoded.data, &encoded.size, 1, decoded.data,
                              PNG_IMAGE_ROW_STRIDE(image), NULL);

    return encoded;
}

void PNGHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
