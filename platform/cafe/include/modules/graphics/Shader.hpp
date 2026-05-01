#pragma once

#include "driver/display/AttributeLayout.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Volatile.hpp"

#include <whb/gfx.h>

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

        void updateBuiltinUniforms(GraphicsBase* graphics);

        void resetAttributes()
        {
            this->layout.reset();
        }

        bool initAttribute(uint32_t bufferIndex, uint32_t offset, uint32_t index, DataFormat format);

        bool initFetchShader();

        ptrdiff_t getHandle() const override;

        struct Program
        {
            GX2VertexShader* vertex;
            GX2PixelShader* pixel;
        };

      private:
        Program program;

        void mapActiveUniforms();

        GX2AttributeLayout layout;
        std::unordered_map<size_t, std::string_view> attributes;

        static constexpr auto INVALIDATE_UNIFORM_BLOCK =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;

        GX2RBuffer transformation;
    };
} // namespace love
