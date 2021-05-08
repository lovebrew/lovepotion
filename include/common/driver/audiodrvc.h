#pragma once

#include "modules/thread/types/lock.h"

namespace love::common::driver
{
    class Audrv
    {
      public:
        Audrv();

        bool IsInitialized() const
        {
            return this->initialized;
        };

      protected:
        bool initialized;
    };
} // namespace love::common::driver
