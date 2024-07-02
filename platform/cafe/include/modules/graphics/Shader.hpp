#pragma once

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Volatile.hpp"

#include <whb/gfx.h>

#include "driver/display/Uniform.hpp"

namespace love
{
    class Shader final : public ShaderBase, public Volatile
    {
      public:
        Shader(StandardShader shader);

        virtual ~Shader();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        ptrdiff_t getHandle() const override;

        void updateBuiltinUniforms(GraphicsBase* graphics, Uniform* uniform);

        uint32_t getPixelSamplerLocation(int index);

      private:
        static constexpr auto INVALIDATE_UNIFORM_BLOCK =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;

        bool validate(const char* filepath, std::string& error);
        WHBGfxShaderGroup program;

        uint32_t modelViewLocation;
        uint32_t projectionLocation;
    };
} // namespace love
