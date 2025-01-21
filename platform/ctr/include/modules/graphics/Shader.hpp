#pragma once

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Volatile.hpp"

#include <3ds.h>

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

        ptrdiff_t getHandle() const override;

        void updateBuiltinUniforms(GraphicsBase* graphics, C3D_Mtx mdlvMtx, const C3D_Mtx& projMtx);

      private:
        void mapActiveUniforms();

        shaderProgram_s program;
    };
} // namespace love
