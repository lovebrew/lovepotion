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
        Shader();

        virtual ~Shader();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void attach() override;

        ptrdiff_t getHandle() const override;

        void updateUniforms(const C3D_Mtx& mdlView, const C3D_Mtx& proj);

      private:
        struct TextureUnit
        {
            C3D_Tex* texture   = nullptr;
            TextureType type   = TextureType::TEXTURE_2D;
            bool isTexelBuffer = false;
            bool active        = false;
        };

        bool validate(const char* filepath, std::string& error);

        DVLB_s* dvlb = nullptr;
        shaderProgram_s program;

        std::vector<uint32_t> data;
        std::vector<TextureUnit> textureUnits;

        Uniform locations;
    };
} // namespace love
