#include "common/Exception.hpp"
#include "common/Color.hpp"
#include "common/math.hpp"

#include "modules/image/magpie/JPGHandler.hpp"

#if defined(__3DS__)
    #include "modules/image/magpie/TileImage.hpp"
#endif

#include <cstddef>
#include <turbojpeg.h>

namespace love
{
    bool JPGHandler::canDecode(Data* data) const
    {
        auto handle = tjInitDecompress();

        if (!handle)
            return false;

        int width, height, samples = 0;
        const auto size = data->getSize();

        if (tjDecompressHeader2(handle, (uint8_t*)data->getData(), size, &width, &height, &samples) < 0)
        {
            tjDestroy(handle);
            return false;
        }

        tjDestroy(handle);
        return true;
    }

    FormatHandler::DecodedImage JPGHandler::decode(Data* data) const
    {
        auto handle = tjInitDecompress();

        if (handle == NULL)
            throw love::Exception("Failed to initialize TurboJPEG decompressor");

        int width, height, samples = 0;
        const auto size = data->getSize();

        if (tjDecompressHeader2(handle, (uint8_t*)data->getData(), size, &width, &height, &samples) < 0)
        {
            tjDestroy(handle);
            throw love::Exception("Failed to read JPEG image header");
        }

        DecodedImage image {};
        image.width  = width;
        image.height = height;
        image.format = PIXELFORMAT_RGBA8_UNORM;
        image.size   = (width * height) * sizeof(uint32_t);
        image.data   = new uint8_t[image.size];

#if defined(__3DS__)
        const auto format = TJPF_ABGR; // Why.
#else
        const auto format = TJPF_RGBA;
#endif
        const auto flags  = TJFLAG_ACCURATEDCT;

        if (tjDecompress2(handle, (uint8_t*)data->getData(), size, image.data, width, 0, height, format,
                          flags) < 0)
        {
            tjDestroy(handle);
            delete[] image.data;
            throw love::Exception("Failed to decompress JPEG image: {:s}", tjGetErrorStr());
        }

        tjDestroy(handle);

#if defined(__3DS__)
    tileImage(image);
#endif

        return image;
    }
} // namespace love
