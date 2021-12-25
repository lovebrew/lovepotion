#pragma once

#include "common/module.h"
#include "objects/file/file.h"

namespace love
{
    class ImageModule : public Module
    {
      public:
        ImageModule();

        virtual ~ImageModule();

        ModuleType GetModuleType() const override
        {
            return M_IMAGE;
        }

        const char* GetName() const override
        {
            return "love.image";
        }

        bool IsCompressed(Data* data);
    };
} // namespace love
