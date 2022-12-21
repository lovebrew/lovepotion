#pragma once

#include <objects/shader/shader.tcc>

#include <3ds.h>

namespace love
{
    template<>
    class Shader<Console::CTR> : public Shader<Console::ALL>
    {
      public:
        Shader();

        Shader(Data* shaderFile);

        virtual ~Shader();

        void Attach();

        static void AttachDefault(StandardShader type);

        bool Validate(Data* data);

        void LoadDefaults(StandardShader type);

      private:
        DVLB_s* shaderFile;
        shaderProgram_s program;
    };
} // namespace love
