#pragma once

#include "deko3d/CShader.h"
#include "objects/shader/shaderc.h"

namespace love
{
    class Shader : public common::Shader
    {
      public:
        struct Program
        {
            std::optional<CShader> vertex;
            std::optional<CShader> fragment;
        };

        virtual void Attach() override;

        virtual void LoadDefaults(StandardShader defaultType) override;

        bool Validate(const CShader& vertex, const CShader& pixel, std::string& error);

        static const char* GetStageName(CShader& shader);

      private:
        Program program;
    };
} // namespace love
