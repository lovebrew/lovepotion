#pragma once

#include "common/module.h"

#include <vector>

namespace love
{
    class Touch : public Module
    {
      public:
        struct TouchInfo
        {
            int64_t id;

            double x;
            double y;

            double dx;
            double dy;

            double pressure;
        };

        ModuleType GetModuleType() const
        {
            return M_TOUCH;
        }

        const char* GetName() const override
        {
            return "love.touch";
        }

        const std::vector<TouchInfo>& GetTouches() const;

        const TouchInfo& GetTouch(int64_t id) const;

        void OnEvent(int type, const TouchInfo& info);

      private:
        std::vector<TouchInfo> touches;
    };
} // namespace love
