#pragma once

#include <array>

namespace love
{
    class SensorBase
    {
      public:
        SensorBase() : enabled(false)
        {}

        virtual std::array<float, 3> GetData() = 0;

        virtual void SetEnabled(bool enable)
        {
            this->enabled = enable;
        }

        bool IsEnabled() const
        {
            return this->enabled;
        }

      protected:
        bool enabled;
    };
} // namespace love
