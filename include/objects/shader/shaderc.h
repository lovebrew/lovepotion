#pragma once

#include "common/data.h"
#include "objects/object.h"
#include <optional>

namespace love::common
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
            STANDARD_VIDEO,
            STANDARD_MAX_ENUM
        };

        // Pointer to currently active Shader.
        static Shader* current;

        // Pointer to the default Shader.
        static Shader* standardShaders[STANDARD_MAX_ENUM];

        virtual void Attach() {};

        virtual void LoadDefaults(StandardShader defaultType) {};

        static void AttachDefault(StandardShader defaultType);

        static bool GetConstant(const char* in, StandardShader& out);
        static bool GetConstant(StandardShader in, const char*& out);

        static bool IsDefaultActive();
        static bool IsActive(StandardShader type);

      protected:
        StandardShader shaderType;
    };
} // namespace love::common
