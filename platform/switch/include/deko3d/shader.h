#pragma once

#include "deko3d/CShader.h"

#include "common/data.h"

#include "common/stringmap.h"
#include "objects/object.h"

#include <optional>

namespace love
{
    class Shader : public Object
    {
      public:
        static love::Type type;

        Shader();

        Shader(love::Data* vertex, love::Data* pixel);

        virtual ~Shader();

        enum StandardShader
        {
            STANDARD_DEFAULT,
            STANDARD_TEXTURE,
            STANDARD_MAX_ENUM
        };

        struct Program
        {
            std::optional<CShader> vertex;
            std::optional<CShader> fragment;
        };

        // Pointer to currently active Shader.
        static Shader* current;

        // Pointer to the default Shader.
        static Shader* standardShaders[STANDARD_MAX_ENUM];

        void Attach();

        bool Validate(const CShader& vertex, const CShader& pixel, std::string& error);

        void LoadDefaults(StandardShader defaultType);

        static void AttachDefault(StandardShader defaultType);

        static const char* GetStageName(CShader& shader);

        static bool GetConstant(const char* in, StandardShader& out);
        static bool GetConstant(StandardShader in, const char*& out);

        static bool IsDefaultActive();

      private:
        Program program;

        const static StringMap<StandardShader, STANDARD_MAX_ENUM> shaderNames;
    };
} // namespace love
