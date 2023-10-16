#pragma once

#include <objects/shader/shader.tcc>

#include <3ds.h>

namespace love
{
    template<>
    class Shader<Console::CTR> : public Shader<Console::ALL>
    {
      public:
        struct Uniforms
        {
            int8_t uLocProjMtx;
            int8_t uLocMdlView;
        };

        Shader();

        Shader(Data* data);

        virtual ~Shader();

        void Attach();

        static void AttachDefault(StandardShader type);

        bool Validate(const char* filepath, std::string& error);

        bool Validate(Data* data, std::string& error);

        void LoadDefaults(StandardShader type);

        Uniforms GetUniformLocations()
        {
            return this->uniforms;
        }

      private:
        DVLB_s* binary;
        shaderProgram_s program;

        std::unique_ptr<uint32_t[]> data;
        Uniforms uniforms;
    };
} // namespace love
