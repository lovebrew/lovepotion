#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <common/data.hpp>

#include <memory>

namespace love
{
    class SystemFont;

    template<Console::Platform T = Console::ALL>
    class FontModule : public Module
    {
      public:
        virtual ~FontModule()
        {}

        virtual ModuleType GetModuleType() const
        {
            return M_FONT;
        }

        virtual const char* GetName() const
        {
            return "love.font";
        }

      protected:
        std::shared_ptr<SystemFont> defaultFont;
    };
} // namespace love
