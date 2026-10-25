#pragma once

#include "modules/image/FormatHandler.hpp"

#include <memory>

namespace love
{
    void tileImage(FormatHandler::DecodedImage& image);
    std::unique_ptr<uint8_t[]> untileImage(const FormatHandler::DecodedImage& image);
}
