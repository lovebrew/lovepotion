#pragma once

#include <vector>

namespace love
{
    class SensorBase
    {
      public:
        SensorBase() : enabled(false)
        {}

        virtual std::vector<float> GetData() = 0;

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
