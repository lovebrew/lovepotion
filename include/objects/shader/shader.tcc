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
        enum StandardShader
        {
            STANDARD_DEFAULT, //< primitives
            STANDARD_TEXTURE, //< textures
            STANDARD_VIDEO,   //< ogg theora videos
            STANDARD_MAX_ENUM
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

        // clang-format off
        static constexpr BidirectionalMap standardShaders = {
            "default", STANDARD_DEFAULT,
            "texture", STANDARD_TEXTURE,
            "video",   STANDARD_VIDEO
        };
        // clang-format on
    };
} // namespace love
