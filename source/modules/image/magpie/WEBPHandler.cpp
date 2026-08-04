#include "modules/image/magpie/WEBPHandler.hpp"

#include "common/Exception.hpp"

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

        return image;
    }

    void WEBPHandler::freeRawPixels(unsigned char* memory)
    {
        delete[] memory;
    }
} // namespace love
