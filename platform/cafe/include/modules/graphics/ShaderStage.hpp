#pragma once

#include "modules/graphics/ShaderStage.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gfx.h>
#include <whb/gfx.h>

namespace love
{
    class ShaderStage final : public ShaderStageBase, public Volatile
    {
      public:
        ShaderStage(ShaderStageType stage, const std::string& filepath);

        virtual ~ShaderStage();

        ptrdiff_t getHandle() const override;

        bool loadVolatile() override;

        void unloadVolatile() override;

      private:
        GX2PixelShader* pixel;
        GX2VertexShader* vertex;
    };
} // namespace love
