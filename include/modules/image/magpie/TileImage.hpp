#pragma once

#include "modules/image/FormatHandler.hpp"

#include <memory>

namespace love
{
    class TileImage
    {
        public:
            static void tile(FormatHandler::DecodedImage& image);
            static void tile(FormatHandler::DecodedImage& image, void (*freeSource)(void*));
            static std::unique_ptr<uint8_t[]> untile(const FormatHandler::DecodedImage& image);

        private:
            static void deletePixelBuffer(void* memory);
    };
}
