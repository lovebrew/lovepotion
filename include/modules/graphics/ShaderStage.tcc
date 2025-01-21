#pragma once

#include "common/Map.hpp"
#include "common/Object.hpp"

#include "modules/graphics/Resource.hpp"

#include <cstring>

#include <stddef.h>

namespace love
{
    class Graphics;

    enum ShaderStageType
    {
        SHADERSTAGE_VERTEX,
        SHADERSTAGE_PIXEL,
        SHADERSTAGE_MAX_ENUM
    };

    enum ShaderStageMask
    {
        SHADERSTAGEMASK_NONE   = 0,
        SHADERSTAGEMASK_VERTEX = 1 << SHADERSTAGE_VERTEX,
        SHADERSTAGEMASK_PIXEL  = 1 << SHADERSTAGE_PIXEL
    };

    class ShaderStageBase : public Object
    {
      public:
        ShaderStageBase(ShaderStageType stage, const std::string& filepath);

        virtual ~ShaderStageBase()
        {}

        virtual ptrdiff_t getHandle() const = 0;

        ShaderStageType getStageType() const
        {
            return this->stageType;
        }

        const std::string& getWarnings() const
        {
            return this->warnings;
        }

        // clang-format off
        STRINGMAP_DECLARE(ShaderStages, ShaderStageType,
            { "vertex", SHADERSTAGE_VERTEX },
            { "pixel",  SHADERSTAGE_PIXEL  }
        )
        // clang-format on

      protected:
        std::string warnings;
        std::string filepath;

      private:
        ShaderStageType stageType;
    };
} // namespace love
