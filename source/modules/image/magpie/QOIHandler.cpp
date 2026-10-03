#include "common/Exception.hpp"
#include "common/pixelformat.hpp"

#include "modules/image/FormatHandler.hpp"
#include "modules/image/magpie/QOIHandler.hpp"

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#include "qoi.h"

#include <cstring>

namespace love
{
    bool QOIHandler::canDecode(Data* data)
    {
        if (data->getSize() < sizeof(Header))
            return false;

        auto* header  = (Header*)data->getData();
        const auto ok = std::strncmp(header->magic, QOIHandler::MAGIC, sizeof(header->magic)) == 0;

        return ok && header->width > 0 && header->height > 0;
    }

    bool QOIHandler::canEncode(PixelFormat rawFormat, EncodedFormat encodedFormat) const
    {
        return encodedFormat == ENCODED_QOI && rawFormat == PIXELFORMAT_RGBA8_UNORM;
    }

    FormatHandler::DecodedImage QOIHandler::decode(Data* data)
    {
        DecodedImage image {};

        qoi_desc description {};
        image.data   = (uint8_t*)qoi_decode(data->getData(), data->getSize(), &description, 4);
        image.width  = description.width;
        image.height = description.height;
        image.size   = image.width * image.height * 4;
        image.format = PIXELFORMAT_RGBA8_UNORM;

        if (image.data == nullptr)
            throw love::Exception("Could not decode image with QOI.");

        return image;
    }

    FormatHandler::EncodedImage QOIHandler::encode(const DecodedImage& image, EncodedFormat encodedFormat)
    {
        EncodedImage encoded {};

        qoi_desc description {};
        description.width      = image.width;
        description.height     = image.height;
        description.channels   = 4;
        description.colorspace = QOI_SRGB;

        int length;
        encoded.data = (uint8_t*)qoi_encode((void*)image.data, &description, &length);
        encoded.size = length;

        if (encoded.data == nullptr)
            throw love::Exception("Could not encode image with QOI.");

        return encoded;
    }

    void QOIHandler::freeRawPixels(uint8_t* memory)
    {
        std::free(memory);
    }

    void QOIHandler::freeEncodedImage(uint8_t* memory)
    {
        std::free(memory);
    }
} // namespace love
