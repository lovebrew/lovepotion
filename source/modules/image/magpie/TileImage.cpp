#include "common/Color.hpp"
#include "common/Exception.hpp"
#include "common/math.hpp"

#include "modules/image/magpie/TileImage.hpp"

#include <memory>

namespace love
{
    void tileImage(FormatHandler::DecodedImage& image)
    {
        const auto width  = static_cast<size_t>(image.width);
        const auto height = static_cast<size_t>(image.height);
        std::unique_ptr<uint8_t[]> sourceData(image.data);

        if (width > LOVE_TEX3DS_MAX || height > LOVE_TEX3DS_MAX)
            throw love::Exception("Texture dimensions exceed 3DS limits.");
        else if (width < LOVE_TEX3DS_MIN || height < LOVE_TEX3DS_MIN)
            throw love::Exception("Texture dimensions are too small.");

        const auto tiledSize = love::getPixelFormatSliceSize(image.format, image.width, image.height, true);
        auto* tiledData = new uint8_t[tiledSize] {};

        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                const auto* source = (const uint32_t*)sourceData.get() + y * image.width + x;
                *colorFromTile<uint32_t>(tiledData, NextPo2(image.width), x, y) = *source;
            }
        }

        image.data = tiledData;
    }
} // namespace love
