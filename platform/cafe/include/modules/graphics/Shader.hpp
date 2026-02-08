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

        bool initAttribute(uint32_t bufferIndex, uint32_t offset, uint32_t index, GX2AttribFormat format);

        bool initFetchShader(size_t enabledBits);

        ptrdiff_t getHandle() const override;

        struct Program
        {
            GX2VertexShader* vertex;
            GX2PixelShader* pixel;
        };

      private:
        static constexpr auto TESSELATION_NONE     = GX2_FETCH_SHADER_TESSELLATION_NONE;
        static constexpr auto TESSELATION_DISCRETE = GX2_TESSELLATION_MODE_DISCRETE;

        Program program;

        void mapActiveUniforms();

        static constexpr auto INVALIDATE_UNIFORM_BLOCK =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;

        bool attributesDirty;
        std::vector<GX2AttribStream> attributes;
        GX2FetchShader shader;
    };
} // namespace love
