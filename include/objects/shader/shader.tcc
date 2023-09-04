#pragma once

#include <common/console.hpp>
#include <common/data.hpp>
#include <common/object.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Shader : public Object
    {
      public:
        static inline int shaderSwitches = 0;

        enum StandardShader
        {
            STANDARD_DEFAULT, //< primitives
            STANDARD_TEXTURE, //< textures
            STANDARD_VIDEO,   //< ogg theora videos
            STANDARD_MAX_ENUM
        };

        enum ShaderStageType
        {
            STAGE_VERTEX,
            STAGE_PIXEL
        };

        static inline Type type = Type("Shader", &Object::type);

        static inline Shader<Console::Which>* current = nullptr;

        static inline Shader<Console::Which>* defaults[StandardShader::STANDARD_MAX_ENUM] {
            nullptr
        };

        struct Program;

        static bool IsDefaultActive()
        {
            for (int index = 0; index < StandardShader::STANDARD_MAX_ENUM; index++)
            {
                if (current == defaults[index])
                    return true;
            }

            return false;
        }

        static bool IsDefaultActive(StandardShader type)
        {
            if (current == defaults[type])
                return true;

            return false;
        }

        // clang-format off
        static constexpr BidirectionalMap standardShaders = {
            "color",   STANDARD_DEFAULT,
            "texture", STANDARD_TEXTURE,
            "video",   STANDARD_VIDEO
        };

        static constexpr BidirectionalMap shaderStages = {
            "vertex", STAGE_VERTEX,
            "pixel",  STAGE_PIXEL
        };
        // clang-format on

      protected:
        StandardShader shaderType;
    };
} // namespace love
