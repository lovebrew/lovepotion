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

        enum StandardShader
        {
            STANDARD_DEFAULT,
            STANDARD_TEXTURE,
            STANDARD_VIDEO,
            STANDARD_MAX_ENUM
        };

        static ShaderBase* current;
        static ShaderBase* standardShaders[STANDARD_MAX_ENUM];

        virtual ~ShaderBase()
        {
            for (int index = 0; index < STANDARD_MAX_ENUM; index++)
            {
                if (this == standardShaders[index])
                    standardShaders[index] = nullptr;
            }

            if (current == this)
                this->attachDefault(STANDARD_DEFAULT);
        }

        struct Uniform
        {
            uint32_t mdlvMtx;
            uint32_t projMtx;
        };

        Uniform getUniforms() const
        {
            return this->locations;
        }

        virtual void attach() = 0;

        static void attachDefault(StandardShader type)
        {
            auto* defaultShader = standardShaders[type];

            if (defaultShader == nullptr)
            {
                current = nullptr;
                return;
            }

            if (current != defaultShader)
                defaultShader->attach();
        }

        static bool isDefaultActive()
        {
            for (int index = 0; index < STANDARD_MAX_ENUM; index++)
            {
                if (current == standardShaders[index])
                    return true;
            }

            return false;
        }

      protected:
        Uniform locations;
    };
} // namespace love
