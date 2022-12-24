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

        Shader(Data* vertex, Data* fragment);

        virtual ~Shader();

        void Attach();

        static void AttachDefault(StandardShader type);

        bool Validate(const char* filepath, WHBGfxShaderGroup& stage, std::string& error) const;

        bool Validate(Data* data, WHBGfxShaderGroup& stage, std::string& error) const;

        void LoadDefaults(StandardShader type);

      private:
        WHBGfxShaderGroup group;
    };
} // namespace love
