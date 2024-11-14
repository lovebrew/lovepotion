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
        struct UniformInfo
        {
            int8_t location;
            std::string name;
        };

        struct Stage
        {
            dk::Shader shader;
            CMemPool::Handle memory;
        };

        struct Program
        {
            Stage vertex;
            Stage fragment;
        } program;

        Shader();

        Shader(StandardShader shader);

        virtual ~Shader();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        ptrdiff_t getHandle() const override;

        const UniformInfo getUniform(const std::string& name) const;

        bool hasUniform(const std::string& name) const;

        void updateBuiltinUniforms(GraphicsBase* graphics);

      private:
        bool validate(Stage& stage, const char* filepath, std::string& error);
    };
} // namespace love
