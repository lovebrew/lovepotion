#pragma once

#include <common/module.hpp>

#include <utilities/driver/events.hpp>

#include <vector>

namespace love
{
    class Touch : public Module
    {
      public:
        virtual ~Touch()
        {}

        ModuleType GetModuleType() const override
        {
            return M_TOUCH;
        };

        const char* GetName() const override
        {
            return "love.touch";
        }

        const std::vector<Finger>& GetTouches() const;

        const Finger& GetTouch(int64_t id) const;

        void OnEvent(SubEventType type, const Finger& info);

      private:
        std::vector<Finger> touches;
    };
} // namespace love
