#pragma once

#include "modules/font/freetype/TrueTypeRasterizer.hpp"
#include "modules/graphics/Font.tcc"

namespace love
{
    class Font : public FontBase
    {
      public:
        Font(Rasterizer* rasterizer, const SamplerState& samplerState);

        virtual void createTexture() override;

        bool loadVolatile() override;
    };
} // namespace love
