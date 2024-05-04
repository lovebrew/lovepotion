#pragma once

#include "common/Map.hpp"
#include "common/Object.hpp"

#include "modules/graphics/Resource.hpp"

#include <map>
#include <string>
#include <vector>

#include <stddef.h>

namespace love
{
    class ShaderBase : public Object, public Resource
    {
      public:
        static Type type;

        static int shaderSwitches;

        enum StandardShader
        {
            STANDARD_DEFAULT,
            STANDARD_TEXTURE,
            STANDARD_VIDEO,
            STANDARD_MAX_ENUM
        };

        static ShaderBase* current;
        static ShaderBase* standardShaders[STANDARD_MAX_ENUM];

        virtual ~ShaderBase();

        virtual void attach() = 0;

        static void attachDefault(StandardShader type);

        static bool isDefaultActive();
    };
} // namespace love
