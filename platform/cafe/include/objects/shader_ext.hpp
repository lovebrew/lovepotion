#pragma once

#include <objects/shader/shader.tcc>

#include <whb/gfx.h>

namespace love
{
    template<>
    class Shader<Console::CAFE> : public Shader<Console::ALL>
    {
      public:
        Shader();

        Shader(Data* group);

        virtual ~Shader();

        void Attach();

        static void AttachDefault(StandardShader type);

        void LoadDefaults(StandardShader type);

        uint32_t GetPixelSamplerLocation(int index);

      private:
        static constexpr auto GX2_FORMAT_VEC2 = GX2_ATTRIB_FORMAT_FLOAT_32_32;
        static constexpr auto GX2_FORMAT_VEC3 = GX2_ATTRIB_FORMAT_FLOAT_32_32_32;
        static constexpr auto GX2_FORMAT_VEC4 = GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32;

        WHBGfxShaderGroup program;
    };
} // namespace love
