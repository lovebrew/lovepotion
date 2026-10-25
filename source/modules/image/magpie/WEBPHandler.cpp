#include "modules/image/magpie/WEBPHandler.hpp"

#include "common/Exception.hpp"
#include "modules/image/magpie/TileImage.hpp"

#include <new>

#define SIMPLEWEBP_IMPLEMENTATION
#include <simplewebp.h>

namespace love
{
    bool WEBPHandler::canDecode(Data* data) const
    {
        simplewebp* swebp = nullptr;
        auto error        = simplewebp_load_from_memory(data->getData(), data->getSize(), nullptr, &swebp);

        if (error == SIMPLEWEBP_NO_ERROR)
            simplewebp_unload(swebp);

        return swebp != nullptr;
    }

    WEBPHandler::DecodedImage WEBPHandler::decode(Data* data) const
    {
        simplewebp* swebp = nullptr;
        auto error        = simplewebp_load_from_memory(data->getData(), data->getSize(), nullptr, &swebp);

        if (error != SIMPLEWEBP_NO_ERROR)
            throw Exception("Failed to load WebP image (%s)", simplewebp_get_error_text(error));

        size_t width, height;
        simplewebp_get_dimensions(swebp, &width, &height);

        size_t imageSize = width * height * 4;

        unsigned char* buffer = new (std::nothrow) unsigned char[imageSize];

        if (!buffer)
            throw Exception("Could not decode WebP image (not enough memory).");

        error = simplewebp_decode(swebp, buffer, nullptr);
        simplewebp_unload(swebp);

        if (error != SIMPLEWEBP_NO_ERROR)
        {
            delete[] buffer;
            throw Exception("Could not decode WebP image (%s).", simplewebp_get_error_text(error));
        }

        DecodedImage image;
        image.format = PIXELFORMAT_RGBA8_UNORM;
        image.width  = width;
        image.height = height;
        image.size   = imageSize;
        image.data   = buffer;

#if defined(__3DS__)
        imageSize = image.size / 4;
        for (size_t i = 0; i < imageSize; i++) {
            unsigned char *p = image.data + 4 * i;
            unsigned char r = p[0], g = p[1], b = p[2], a = p[3];
            p[0] = a;
            p[1] = b;
            p[2] = g;
            p[3] = r;
        }

        tileImage(image);
#endif

        return image;
    }

    void WEBPHandler::freeRawPixels(unsigned char* memory)
    {
        delete[] memory;
    }
} // namespace love
