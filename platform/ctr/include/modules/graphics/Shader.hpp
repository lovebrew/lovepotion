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
        struct UniformInfo
        {
            int8_t location;
            std::string name;
        };

        Shader();

        virtual ~Shader();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        ptrdiff_t getHandle() const override;

        const UniformInfo getUniform(const std::string& name) const;

        bool hasUniform(const std::string& name) const;

        void updateBuiltinUniforms(const C3D_Mtx& mdlvMtx, const C3D_Mtx& projMtx);

      private:
        bool validate(const char* filepath, std::string& error);

        DVLB_s* dvlb = nullptr;
        shaderProgram_s program;

        std::vector<uint32_t> data;
        UniformInfo uniforms[0x02];
    };
} // namespace love
