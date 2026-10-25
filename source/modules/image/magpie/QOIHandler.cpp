#include "common/Exception.hpp"
#include "common/pixelformat.hpp"

#include "modules/image/FormatHandler.hpp"
#include "modules/image/magpie/TileImage.hpp"
#include "modules/image/magpie/QOIHandler.hpp"

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#include "qoi.h"

#include <cstdlib>
#include <cstring>

namespace love
{
    bool QOIHandler::canDecode(Data* data) const
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

    FormatHandler::DecodedImage QOIHandler::decode(Data* data) const
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

#if defined(__3DS__)
        size_t imageSize = image.size / 4;
        for (size_t i = 0; i < imageSize; i++) {
            unsigned char *p = image.data + 4 * i;
            unsigned char r = p[0], g = p[1], b = p[2], a = p[3];
            p[0] = a;
            p[1] = b;
            p[2] = g;
            p[3] = r;
        }

        tileImage(image, std::free);
#endif

        return image;
    }

    FormatHandler::EncodedImage QOIHandler::encode(const DecodedImage& image, EncodedFormat encodedFormat) const
    {
        if (!canEncode(image.format, encodedFormat))
            throw love::Exception("Cannot encode image to QOI.");

        EncodedImage encoded {};

        qoi_desc description {};
        description.width      = image.width;
        description.height     = image.height;
        description.channels   = 4;
        description.colorspace = QOI_SRGB;

#if defined(__3DS__)
        auto linearData = untileImage(image);
        for (size_t i = 0; i < image.size; i += 4)
        {
            uint8_t r = linearData[i];
            uint8_t g = linearData[i + 1];
            linearData[i] = linearData[i + 3];
            linearData[i + 1] = linearData[i + 2];
            linearData[i + 2] = g;
            linearData[i + 3] = r;
        }
        int encodedSize = 0;
        encoded.data = (uint8_t*)qoi_encode(linearData.get(), &description, &encodedSize);
#else
        int encodedSize = 0;
        encoded.data = (uint8_t*)qoi_encode(image.data, &description, &encodedSize);
#endif

        if (encoded.data == nullptr)
            throw love::Exception("Could not encode image with QOI.");

        encoded.size = (size_t)encodedSize;
        return encoded;
    }

    void QOIHandler::freeRawPixels(uint8_t* memory)
    {
#if defined(__3DS__)
        delete[] memory;
#else
        std::free(memory);
#endif
    }

    void QOIHandler::freeEncodedImage(uint8_t* memory)
    {
        std::free(memory);
    }
} // namespace love
