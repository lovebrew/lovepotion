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
        Shader(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM], const CompileOptions& options);

        virtual ~Shader();

        static const char* getDefaultStagePath(StandardShader shader, ShaderStageType stage);

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        std::string getWarnings() const;

        void updateBuiltinUniforms(GraphicsBase* graphics, Uniform* uniform);

        ptrdiff_t getHandle() const override;

      private:
        void mapActiveUniforms();

        bool setShaderStages(WHBGfxShaderGroup* group, std::array<StrongRef<ShaderStageBase>, 2> stages);

        static constexpr auto INVALIDATE_UNIFORM_BLOCK =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;

        WHBGfxShaderGroup program;
    };
} // namespace love
