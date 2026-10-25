#include "common/Color.hpp"
#include "common/Exception.hpp"
#include "common/math.hpp"

#include "modules/image/magpie/TileImage.hpp"

#include <cstring>
#include <memory>

namespace love
{
    static void deletePixelBuffer(void* memory)
    {
        delete[] (uint8_t*)memory;
    }

    void tileImage(FormatHandler::DecodedImage& image)
    {
        tileImage(image, deletePixelBuffer);
    }

    void tileImage(FormatHandler::DecodedImage& image, void (*freeSource)(void*))
    {
        const auto width  = static_cast<size_t>(image.width);
        const auto height = static_cast<size_t>(image.height);
        std::unique_ptr<uint8_t, void (*)(void*)> sourceData(image.data, freeSource);

        if (width > LOVE_TEX3DS_MAX || height > LOVE_TEX3DS_MAX)
            throw love::Exception("Texture dimensions exceed 3DS limits.");
        else if (width < LOVE_TEX3DS_MIN || height < LOVE_TEX3DS_MIN)
            throw love::Exception("Texture dimensions are too small.");

        const auto tiledSize = love::getPixelFormatSliceSize(image.format, image.width, image.height, true);
        auto* tiledData = new uint8_t[tiledSize] {};
        auto widthPo2 = NextPo2(image.width);

        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                const auto* source = (const uint32_t*)sourceData.get() + y * image.width + x;
                *colorFromTile<uint32_t>(tiledData, widthPo2, x, y) = *source;
            }
        }

        image.data = tiledData;
    }

    std::unique_ptr<uint8_t[]> untileImage(const FormatHandler::DecodedImage& image)
    {
        const auto widthPo2 = NextPo2(image.width);
        auto linearData = std::make_unique<uint8_t[]>(image.size);

        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                const auto* tiled = colorFromTile<uint32_t>(image.data, widthPo2, x, y);
                auto* pixel = linearData.get() + (y * image.width + x) * 4;
                std::memcpy(pixel, tiled, 4);
            }
        }

        return linearData;
    }
} // namespace love
