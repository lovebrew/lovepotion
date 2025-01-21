#pragma once

#include "driver/display/deko3d/CMemPool.h"
#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Volatile.hpp"

#include <deko3d.hpp>

namespace love
{
    class Shader : public ShaderBase, public Volatile
    {
      public:
        struct Program
        {
            dk::Shader vertex;
            dk::Shader fragment;
        };

        Shader(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM], const CompileOptions& options);

        static const char* getDefaultStagePath(StandardShader shader, ShaderStageType stage);

        virtual ~Shader();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        ptrdiff_t getHandle() const override;

        void updateBuiltinUniforms(GraphicsBase* graphics);

      private:
        void mapActiveUniforms();

        Program program;
    };
} // namespace love
