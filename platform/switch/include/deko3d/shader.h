#pragma once

#include "deko3d/CShader.h"

#include "common/data.h"

#include "objects/object.h"

namespace love
{
    class Shader : public Object
    {
        public:
            static love::Type type;

            Shader();

            Shader(love::Data * vertex, love::Data * pixel);

            ~Shader();

            enum StandardShader
            {
                STANDARD_DEFAULT,
                STANDARD_TEXTURE,
                STANDARD_MAX_ENUM
            };

            struct Program
            {
                CShader vertex;
                CShader fragment;
            };

            // Pointer to currently active Shader.
            static Shader * current;

            // Pointer to the default Shader.
            static Shader * standardShaders[STANDARD_MAX_ENUM];

            void Attach();

            bool Validate(const CShader & vertex, const CShader & pixel, std::string & error);

            void LoadDefaults(StandardShader defaultType);

            static void AttachDefault(StandardShader defaultType);

            static bool IsDefaultActive();

        private:
            Program program;
    };
}