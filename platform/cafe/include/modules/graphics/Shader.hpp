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
        struct UniformInfo
        {
            uint32_t location;
            std::string name;
        };

        Shader(StandardShader shader);

        virtual ~Shader();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        ptrdiff_t getHandle() const override;

        const UniformInfo getUniform(const std::string& name) const;

        bool hasUniform(const std::string& name) const;

        void updateBuiltinUniforms(GraphicsBase* graphics, Uniform* uniform);

        uint32_t getPixelSamplerLocation(int index);

      private:
        static constexpr auto INVALIDATE_UNIFORM_BLOCK =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;

        bool validate(const char* filepath, std::string& error);
        WHBGfxShaderGroup program;

        UniformInfo uniform;
    };
} // namespace love
