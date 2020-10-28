#pragma once

#include "deko3d/shaderstage.h"
#include "deko3d/deko.h"

namespace love
{
    class Shader : public Object
    {
        public:
            static love::Type type;

            Shader(ShaderStage * vertex, ShaderStage * pixel);
            ~Shader();

            enum StandardShader
            {
                STANDARD_DEFAULT,
                STANDARD_VIDEO,
                STANDARD_ARRAY,
                STANDARD_MAX_ENUM
            };

            // Pointer to currently active Shader.
            static Shader * current;

            // Pointer to the default Shader.
            static Shader * standardShaders[STANDARD_MAX_ENUM];

            void Attach();

            bool Validate(ShaderStage * vertex, ShaderStage * pixel, std::string & err);

            static void AttachDefault(StandardShader defaultType);

            static bool IsDefaultActive();

        protected:
            StrongReference<ShaderStage> stages[ShaderStage::STAGE_MAX_ENUM];

        private:
            bool canvasWasActive;
            Rect lastViewport;

            float lastPointSize;

            Matrix4 lastTransformMatrix;
            Matrix4 lastProjectionMatrix;
    };
}